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

// ピクセルシェーダー
// 画面の1pixelごとにどんな色を表示するか計算する
// 戻り値のfloat4はそれぞれ、R,G,B,Aを示す
// float4(0,0,0,0)なら黒、float4(1,1,1,1)なら白
float4 main(PS_IN pin) : SV_TARGET
{
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // テクスチャの色情報を取得
    color = tex.Sample(samp,pin.uv);

    return color;
}