// CPUから受け取るモデルの頂点情報
// 受け取る予定の頂点と一致させておく
struct VS_IN
{
    float3 pos : POSITIONT;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    uint instanceID : SV_InstanceID;
};

// ピクセルシェーダーに渡すデータの塊(渡す先のピクセルシェーダーと一致させる)
struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPos : POSITION0;
};

struct PerInstanceData
{
    float3 pos;
    float dummy;
    float3 scale;
    float dummy2;
    float4 quaternion;
};

// インスタンシング用のデータ
StructuredBuffer<PerInstanceData> g_pInstanceData : register(t0);

// ● 定数バッファ
// cbuffer(constant buffer:定数バッファ)
// モデルの表示に必要な座標やUVといったデータは
// セマンティクスで細かく指定するが、それら以外の
// データは定数バッファにまとめて、GPUのグラフィックスメモリ上に配置される。
// メモリ上のバッファとして用意されている領域の
// 何番目に配置されているかをregister(b0)のような形で示す
cbuffer ModelMatrix : register(b0)
{
    float4x4 world; // ワールド行列
    float4x4 view; // ビュー行列
    float4x4 proj; // プロジェクション行列
};

// ● main関数の引数
// 描画で使用するモデルの頂点情報
// 引数で渡された頂点が画面のどの位置に表示されるか
// main関数の内部で計算される
// ● セマンティクス
// GPUに渡しているデータの使用用途を明示的に示すための記述
VS_OUT main(VS_IN vin)
{
    VS_OUT vout;
    PerInstanceData inst = g_pInstanceData[vin.instanceID];

    // スケール適用
    float3 scaledPos = vin.pos * inst.scale;

    // クォータニオンから回転行列生成
    float4 q = inst.quaternion;
    float3x3 rotMat = float3x3(
        1 - 2 * (q.z * q.z + q.w * q.w), 2 * (q.y * q.z - q.w * q.x), 2 * (q.y * q.w + q.z * q.x),
        2 * (q.y * q.z + q.w * q.x), 1 - 2 * (q.y * q.y + q.w * q.w), 2 * (q.z * q.w - q.y * q.x),
        2 * (q.y * q.w - q.z * q.x), 2 * (q.z * q.w + q.y * q.x), 1 - 2 * (q.y * q.y + q.z * q.z)
    );
    float3 rotatedPos = mul(rotMat, scaledPos);

    // 相対位置（インスタンスごとのオフセット）を加算
    float3 worldPos = rotatedPos + inst.pos;

    // 以降は今まで通り
    vout.pos = float4(worldPos, 1.0f);
    vout.pos = mul(world, vout.pos);
    vout.worldPos = vout.pos;
    vout.pos = mul(view, vout.pos);
    vout.pos = mul(proj, vout.pos);
    vout.uv = vin.uv;
    vout.normal = mul((float3x3) world, vin.normal);

    return vout;
}
