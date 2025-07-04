// 頂点シェーダーから受け取るデータ
struct PS_IN
{
    float4 pos : SV_Position; // 画面の位置
    float2 uv : TEXCOORD0; // テクスチャから読み取るUV座標
    float3 normal : NORMAL0;
};


// モデルの形は同じでも色は異なる(格ゲーの2Pカラー
// モデルごとに貼り付けるテクスチャが異なるため、
// 頂点シェーダーの定数バッファと同じように外部から
// 使用するテクスチャを渡す
Texture2D tex : register(t0);

// サンプラー
// テクスチャの拡縮方法、繰り返し設定のデータ
SamplerState samp : register(s0);

cbuffer time : register(b0)
{
    float1 time;
}


// ピクセルシェーダー
// 画面の1pixelごとにどんな色を表示するか計算する
// 戻り値のfloat4はそれぞれ、R,G,B,Aを示す
// float4(0,0,0,0)なら黒、float4(1,1,1,1)なら白
float4 main(PS_IN pin) : SV_TARGET
{
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // テクスチャの色情報を取得
    color = tex.Sample(samp,pin.uv);
    float1 work1;
    float1 work2;
    float1 work3;
    
    work1 = color.r * color.r;
    work2 = color.b / 2.0f;
    work3 = color.g % 0.3f;
    
    if(time >= 0.8f)
    {
        color.r = color.b / 2.0f;
        color.b = color.g % 0.3f;
        color.g = work1;
    }
    else
    {
        if (color.r > work2)
            color.r -= time;
        else if (color.r < work2)
            color.r += time;
    
        if (color.g > work1)
            color.g -= time;
        else if (color.g < work1)
            color.g += time;
    
        if (color.b > work3)
            color.b -= time;
        else if (color.b < work3)
            color.b += time;
    }

    return color;
}