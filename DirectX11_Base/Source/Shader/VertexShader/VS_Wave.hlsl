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

VS_OUT main(VS_IN vin)
{
	VS_OUT vout;
	vout.pos = float4(vin.pos, 1.0f);
	
    // 画面の座標へ変換するための処理
	vout.pos = mul(world, vout.pos); // ローカル → ワールドへ変換
	vout.worldPos = vout.pos; // 計算途中のデータを格納
	vout.pos = mul(view, vout.pos); // ワールド → ビューへ変換
	vout.pos = mul(proj, vout.pos); // ビュー → プロジェクションへ変換
    
    // UVのデータはそのままピクセルシェーダーに渡す
	vout.uv = vin.uv;
    
    // 法線はモデルが回転している場合、
    // ピクセルシェーダーに渡す前に同じ回転をかけておく。
    // float4x4では移動の成分も含まれるため、
    // キャストすることで移動成分がない行列が作成できる
	vout.normal = mul((float3x3) world, vin.normal);
	
    // サイン波の計算（頂点位置を動かす）
	float generalWave = sin(vout.worldPos.x * waveFrequency + time * 1.0f) +
						cos(vout.worldPos.z * waveFrequency + time * 1.0f);
	
	//float wave = sin(input.pos.x * waveFrequency + time * waveSpeed);
	vout.pos.y += generalWave * waveAmplitude;

	return vout;
}