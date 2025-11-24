struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 worldPos : POSITION0;
    // スクリーン上の位置を表すデータ
    float4 screenPos : POSITION1;
};

float4 main(PS_IN pin) : SV_TARGET
{
    // zの値にはワールド空間上でのカメラと表示しようとしている
    // ピクセルまでの距離が格納されているが、そのままでは小数点以下の
    // 計算で誤差が出る。wの値に格納されている表示できる限界の距離で
    // zを割ると0~1の範囲に正規化されるのでその値を深度値として描き込む
	float depth = pin.screenPos.z / pin.screenPos.w;
    return float4(
        depth,
        depth,
        depth, 1.0f);
}