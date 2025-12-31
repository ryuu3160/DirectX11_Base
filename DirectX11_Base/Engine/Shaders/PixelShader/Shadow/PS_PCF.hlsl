// 頂点シェーダーから受け取るデータ
struct PS_IN
{
	float4 pos : SV_Position; // 画面の位置
	float2 uv : TEXCOORD0; // テクスチャから読み取るUV座標
	float4 color : TEXCOORD1;
	float3 wPos : POSITION0;
	float4 screenPos : POSITION1;
};

Texture2D tex : register(t0);
Texture2D ShadowMap : register(t1);
SamplerState samp : register(s0);
SamplerComparisonState ShadowSample : register(s1);

// ワールド座標を光源のカメラ位置に移動させる
// 行列変換
cbuffer LightVP : register(b0)
{
	float4x4 LightView;
	float4x4 LightProj;
}

float4 main(PS_IN pin) : SV_TARGET
{
	float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	color = tex.Sample(samp, pin.uv);

    // ゲームのカメラに合わせてシャドウjマップを貼り付ける場合、
    // モデルのUVを利用するのではなく光源のカメラから見たときの画面座標を
    // UVとして利用する。

    // 1.ワールド座標を光源のカメラ座標に変換
	float4 LightPos = float4(pin.wPos, 1.0f);
	LightPos = mul(LightPos, LightView);
	LightPos = mul(LightPos, LightProj);

    // 2.画面の座標は-1～1のため、0～1に変換
	float2 LightUV = LightPos.xy * 0.5f + 0.5f;
    // 画面座標は上方向が＋、UV座標は下方向が＋のため、Y軸のみ反転
	LightUV.y = 1.0f - LightUV.y;

	// 2.描画しようとしているピクセルの奥行きを取得
	float WriteDepth = LightPos.z / LightPos.w;
    // シャドウアクネ対策で実際の表示よりも手前にあるものとして計算
	WriteDepth -= 0.005f;

	// PCFでは近傍4ピクセルから深度値と比較して遮蔽率を計算
	// 遮蔽率に王子て影の濃淡を変更する
	// HLSLでは組み込み関数が用意されているため、その関数を利用して計算する
	float4 Shadow = ShadowMap.SampleCmpLevelZero(
	   ShadowSample, LightUV, WriteDepth);

	// 遮蔽率をもとに影の割合を計算
	color.rgb = lerp(color.rgb, float3(0.2f, 0.2f, 0.2f), Shadow.x);

	return color;
}