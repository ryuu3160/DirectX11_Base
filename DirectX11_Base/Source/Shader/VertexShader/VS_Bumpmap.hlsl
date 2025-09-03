struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    // タンジェントベクトル(接ベクトル
    // タンジェント空間上のX軸方向を表す
    float3 tangent : TANGENT0;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPos : POSITION0;
    float3 tangent : TANGENT0;
};

cbuffer ModelMatrix : register(b0)
{
    float4x4 world; // ワールド行列
    float4x4 view; // ビュー行列
    float4x4 proj; // プロジェクション行列
};

VS_OUT main(VS_IN vin)
{
    VS_OUT vout;
    vout.pos = float4(vin.pos, 1.0f);
    vout.pos = mul(world, vout.pos);
    vout.worldPos = vout.pos;
    vout.pos = mul(view, vout.pos);
    vout.pos = mul(proj, vout.pos);
    vout.uv = vin.uv;
    vout.normal = mul((float3x3) world, vin.normal);
    
    // タンジェントベクトルも法線ベクトルと同様に
    // ワールド行列の回転を適用する
    vout.tangent = mul((float3x3) world, vin.tangent);

    return vout;
}
