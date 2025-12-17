struct VS_IN
{
	float3 pos : POSITIONT;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
	uint instanceID : SV_InstanceID;
};
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
	
	// サイン波の計算（頂点位置を動かす）
	float generalWave = sin(worldPos.x * waveFrequency + time * 1.0f) +
						cos(worldPos.z * waveFrequency + time * 1.0f);
	
	vout.pos.y += generalWave * waveAmplitude;

	return vout;
}
