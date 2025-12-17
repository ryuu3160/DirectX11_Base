struct VS_IN
{
	float3 pos : POSITIONT;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
};
struct VS_OUT
{
	float4 pos : SV_Position;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL0;
	float3 worldPos : POSITION0;
};

cbuffer ModelMatrix : register(b0)
{
	float4x4 world; // ワールド行列
	float4x4 view; // ビュー行列
	float4x4 proj; // プロジェクション行列
};

cbuffer WaveInfo : register(b1)
{
	float time; // 時間を渡すための変数
	float waveAmplitude; // 波の振幅
	float waveFrequency; // 波の周期（頻度）
	float waveSpeed; // 波の速度
}

VS_OUT VS(VS_IN input)
{
	VS_OUT output;

    // サイン波の計算（頂点位置を動かす）
	float wave = sin(input.pos.x * waveFrequency + time * waveSpeed);
	input.pos.y += wave * waveAmplitude;

    // 座標変換
	float4 worldPos = mul(float4(input.pos, 1.0), world);
	float4 viewPos = mul(worldPos, view);
	output.pos = mul(viewPos, proj);

	output.uv = input.uv; // UV座標はそのまま渡す
	
	//float generalWave = sin(worldPosition.x * GeneralFrequency + Time * GeneralSpeed) +
 //                       cos(worldPosition.z * GeneralFrequency + Time * GeneralSpeed);

	return output;
}