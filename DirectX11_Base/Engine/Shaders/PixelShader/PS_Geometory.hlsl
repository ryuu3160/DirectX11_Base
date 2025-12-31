struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
};
float4 main(PS_IN pin) : SV_TARGET0
{
    return pin.color;
}