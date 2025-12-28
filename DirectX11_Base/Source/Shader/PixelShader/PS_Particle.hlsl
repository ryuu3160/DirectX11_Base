struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float4 Color : COLOR0;
};

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);


float4 main(PS_INPUT input) : SV_TARGET
{
    // テクスチャサンプリング
    float4 texColor = txDiffuse.Sample(samLinear, input.TexCoord);
    
    // パーティクルの色と乗算
    float4 finalColor = texColor * input.Color;
    
    // アルファ値が非常に小さい場合はピクセルを破棄
    if (finalColor.a < 0.01f)
        discard;
    
    return finalColor;
}