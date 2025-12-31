// 頂点シェーダーから受け取るデータ
struct PS_IN
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPos : POSITION0;
    float3 tangent : TANGENT0;
};

cbuffer Camera : register(b0)
{
    float3 eyePos;
    float camDummy;
}

cbuffer Light : register(b1)
{
    float3 LightDir;
    float dummy;
    float4 LightDiffuse;
    float4 LightAmbient;
};

cbuffer PBR : register(b5)
{
    float metallic;
    float smooth;
    float2 PBRDummy;
}

cbuffer ParallaxSetting : register(b6)
{
    float heightScale;
    int Steps;
    float2 padding;
};


SamplerState samp : register(s0);
Texture2D tex : register(t0);
Texture2D heightMap : register(t4);
Texture2D normalMap : register(t5);

static const float PI = 3.14159265358979323846f; // 円周率

// D項(マイクロファセット)
// ベックマン分布の公式を利用
// m - マイクロファセット(粗さ
// t - 法線とハーフベクトルの近似値
float Beckmann(float m, float t)
{
    float t2 = t * t;
    float t4 = t * t * t * t;
    float m2 = m * m;
    float D = 1.0f / (4.0f * m2 * t4);
    D *= exp((-1.0f / m2) * (1.0f - t2) / t2);
    return D;
}

// G項(幾何減衰)
// NH - 法線とハーフベクトルの内積
// NV - 法線とベクトルの内積
// VH - 法線とベクトルのハーフベクトルの内積
// NL - 法線とライとの向きの内積
float Smith(float NH, float NV, float VH, float NL)
{
    float G1 = 2.0f * NH * NV / VH;
    float G2 = 2.0f * NH * NL / VH;
    float G = min(1.0f, min(G1, G2));
    return G;
}

// F項(フレネル反射)
// f0 - 垂直入射時の反射率
// VH - 視線ベクトルとハーフベクトルの内積
float Fresnel(float f0, float VH)
{
    float F = f0 + (1.0f - f0) * pow(1.0f - VH, 5.0f);
    return F;
}

// CookTorranceモデルの鏡面反射
// L - 光源に向かうベクトル
// V - 視点に向かうベクトル
// N - 法線
// metallic - 金属度合い
// smooth - 滑らかさ
float CookTorrance(float3 L, float3 V, float3 N, float metallic,float smooth)
{
    // ハーフベクトル
    // 光源に向かうベクトルと視点に向かうベクトルの中間のベクトル
    float3 H = normalize(L + V);
    
    // 各種ベクトルの内積(角度)を計算
    float NH = saturate(dot(N, H));
    float VH = saturate(dot(V, H));
    float NL = saturate(dot(N, L));
    float NV = saturate(dot(N, V));
    
    // CookTorranceの各項を計算
    float D = Beckmann(smooth, NH);
    float G = Smith(NH, NV, VH, NL);
    float F = Fresnel(metallic, VH);
    
    // CookTorranceの計算
    return max(D * G * F / (4.0f * NV * NH), 0.0f);
}

// 視差遮蔽マッピング関数
float2 ParallaxOcclusionMapping(float2 uv,float3 viewDirTS)
{
    // 視線角度でサンプル数を可変に
    int steps = Steps;
    float stepSize = 1.0 / steps;
    float2 shift = (viewDirTS.xy / (abs(viewDirTS.z) + 1e-5f)) * heightScale * stepSize;
    float currLayer = 1.0;
    float prevLayer = 1.0f;
    float2 currUV = uv;
    float2 prevUV = uv;
    float currHeight = heightMap.Sample(samp, currUV).r;
    float prevHeight = currHeight;

    [loop]
    for (int i = 0; i < steps; ++i)
    {
        prevUV = currUV;
        prevHeight = currHeight;
        prevLayer = currLayer;
        currLayer -= stepSize;
        
        // 現在のUV座標を計算
        currUV += shift;
        currHeight = heightMap.Sample(samp, currUV).r;
        if (currHeight >= currLayer)
            break;
    }
    
    // 線形補完
    float prev = prevHeight - prevLayer;
    float weight = prev / (prev - (currHeight - currLayer));
    float2 finalUV = lerp(prevUV, currUV, saturate(weight));
    return finalUV;
}

float4 main(PS_IN pin) : SV_TARGET
{
    // TBN行列の作成
    // 法線マップから取得した法線を回転させるための
    // 行列(姿勢行列)を作成
    float3 N = normalize(pin.normal);
    float3 T = normalize(pin.tangent);
    // バイノーマル(従法線)
    // タンジェントスペース上のY軸方向
    float3 B = normalize(cross(T, N));
    
    // 姿勢行列(逆行列)
    float3x3 TBN = float3x3(
        T.x, B.x, N.x,
        T.y, B.y, N.y,
        T.z, B.z, N.z
    );

    // カメラからピクセルへのベクトル（ワールド空間）
    float3 viewDir = normalize(pin.worldPos - eyePos);

    // タンジェント空間へ変換
    float3 viewDirTS = mul(TBN, viewDir);

    // POM関数に渡す
    float2 uv = ParallaxOcclusionMapping(pin.uv, viewDirTS);

    // 法線マップから法線を取得
    float3 nMap = normalMap.Sample(samp, uv).rgb * 2.0f - 1.0f;
    float3 Nw = mul(TBN, normalize(nMap));
    
    float4 color4 = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // ライト計算
    float3 L = LightDir;
    float t = dot(Nw, L);
    t *= -1.0f;
    t = saturate(t);
    
    // 正規化拡散反射
    // 物理的に正しい拡散反射を計算
    t /= PI;
    
    // CookTorranceを利用した鏡面反射
    float3 V = normalize(eyePos - pin.worldPos);
    float3 spec = CookTorrance(-L, V, Nw, metallic, smooth);
    spec *= LightDiffuse.rgb;

    // テクスチャの色情報を取得
    color4 = tex.Sample(samp, pin.uv);
    
    // 滑らかさが高ければ、鏡面反射に上書きされて拡散反射は弱くなる
    float3 diffuse = t * LightDiffuse.rgb;
    diffuse *= 1.0f - metallic;
    
    float3 ambient = LightAmbient.rgb * 0.2f;

    float4 baseColor = tex.Sample(samp, uv);
    float3 color = baseColor.rgb * (LightDiffuse.rgb * diffuse + spec + ambient);

    return float4(color, baseColor.a);
}