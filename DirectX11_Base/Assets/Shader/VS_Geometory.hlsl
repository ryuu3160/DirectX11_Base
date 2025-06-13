struct VS_IN
{
    float3 pos : POSITION0;
    float4 color : COLOR0;
    float3 normal : NORMAL0;
};
struct VS_OUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
};
cbuffer GeometoryMatrix : register(b0) {
    float4x4 world;
    float4x4 view;
    float4x4 proj;
};
cbuffer Param : register(b1)
{
    float4 color;
    float4 light;
};
VS_OUT main(VS_IN vin)
{
    VS_OUT vout;
    vout.pos = float4(vin.pos, 1.0f);
    vout.pos = mul(vout.pos, world);
    vout.pos = mul(vout.pos, view);
    vout.pos = mul(vout.pos, proj);
	
    float3 N = normalize(mul(vin.normal, (float3x3) world));
    float3 L = normalize(light.xyz);
    float diffuse = dot(N, L) * 0.5f + 0.5f;
    diffuse = saturate(diffuse + light.w);
    vout.color = vin.color * color * diffuse;

    return vout;
}