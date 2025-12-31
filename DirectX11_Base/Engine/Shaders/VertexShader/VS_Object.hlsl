// CPUから受け取るモデルの頂点情報
// 受け取る予定の頂点と一致させておく
struct VS_IN
{
    float3 pos : POSITIONT;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};

// ピクセルシェーダーに渡すデータの塊(渡す先のピクセルシェーダーと一致させる)
struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPos : POSITION0;
};

// ● 定数バッファ
// cbuffer(constant buffer:定数バッファ)
// モデルの表示に必要な座標やUVといったデータは
// セマンティクスで細かく指定するが、それら以外の
// データは定数バッファにまとめて、GPUのグラフィックスメモリ上に配置される。
// メモリ上のバッファとして用意されている領域の
// 何番目に配置されているかをregister(b0)のような形で示す
cbuffer ModelMatrix : register(b0)
{
    float4x4 world; // ワールド行列
    float4x4 view;	// ビュー行列
    float4x4 proj;	// プロジェクション行列
};

//cbuffer Time : register(b1)
//{
//    float time; // 時間
//};

// ● main関数の引数
// 描画で使用するモデルの頂点情報
// 引数で渡された頂点が画面のどの位置に表示されるか
// main関数の内部で計算される
// ● セマンティクス
// GPUに渡しているデータの使用用途を明示的に示すための記述
VS_OUT main( VS_IN vin)
{
    VS_OUT vout;
    
    //float waveAmplitude = 0.5f; // 波の振幅
    //float waveFrequency = 2.0f; // 波の周波数
    //float waveSpeed = 1.0f; // 波の速度

    // 頂点のY座標を変更
    //vin.pos.y += waveAmplitude * sin(waveFrequency * vin.pos.x * time + waveSpeed * time);
    
    vout.pos = float4(vin.pos, 1.0f);
	
	// 画面の座標へ変換するための処理
    vout.pos = mul(world, vout.pos);    // ローカル → ワールドへ変換
    vout.worldPos = vout.pos;           // 計算途中のデータを格納
    vout.pos = mul(view, vout.pos);     // ワールド → ビューへ変換
    vout.pos = mul(proj, vout.pos);     // ビュー → プロジェクションへ変換
    
    // UVのデータはそのままピクセルシェーダーに渡す
    vout.uv = vin.uv;
    
    // 法線はモデルが回転している場合、
    // ピクセルシェーダーに渡す前に同じ回転をかけておく。
    // float4x4では移動の成分も含まれるため、
    // キャストすることで移動成分がない行列が作成できる
    vout.normal = mul((float3x3) world, vin.normal);
	
    // 計算された頂点はラスタライザやピクセルの計算に
    // 使用されていく
	return vout;
}
