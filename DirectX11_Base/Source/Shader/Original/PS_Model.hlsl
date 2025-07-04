struct PS_IN
{
    float4 pos : SV_POSITION;
};

float4 main(PS_IN pin) : SV_TARGET
{
    return float4(1.0f, 0.0f, 1.0f, 1.0f);
}