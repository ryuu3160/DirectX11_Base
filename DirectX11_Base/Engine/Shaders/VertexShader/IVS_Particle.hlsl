struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
    uint InstanceID : SV_InstanceID; // インスタンスID
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float4 Color : COLOR0;
};

cbuffer ConstantBuffer : register(b0)
{
    matrix World; // 使用しない (単位行列)
    matrix View;
    matrix Projection;
}

struct InstanceData
{
    float4x4 World; // パーティクル個別のワールド行列
    float4 Color; // パーティクルの色
};

StructuredBuffer<InstanceData> InstanceBuffer : register(t0);


VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    // インスタンスデータ取得
    InstanceData inst = InstanceBuffer[input.InstanceID];
    
    // ビルボード計算
    // ビュー行列の回転成分を抽出 (右、上、前ベクトル)
    float3 cameraRight = float3(View[0][0], View[1][0], View[2][0]);
    float3 cameraUp = float3(View[0][1], View[1][1], View[2][1]);
    
    // パーティクルの中心位置
    float3 particlePos = inst.World[3].xyz;
    
    // パーティクルのスケール
    float2 particleSize = float2(
        length(inst.World[0].xyz),
        length(inst.World[1].xyz)
    );
    
    // ビルボード頂点の計算
    float3 worldPos = particlePos
        + cameraRight * input.Position.x * particleSize.x
        + cameraUp * input.Position.y * particleSize.y;
    
    float4 viewPos = mul(float4(worldPos, 1.0f), View);
    output.Position = mul(viewPos, Projection);
    
    output.TexCoord = input.TexCoord;
    output.Color = inst.Color;
    
    return output;
}