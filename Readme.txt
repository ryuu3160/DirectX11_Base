このフレームワークはSPの授業で使用しているものを改造して作成したものです

=== 基本事項 ===
・ソリューションファイル(.sln)と同じ階層にある、ProjectSetUp.exeを起動して、任意の名前を入力することで、
　プロジェクト名を変更することが出来ます。

・読みこんだモデル(fbxファイル)と同じ階層に作成される、fbx名_MaterialShaderInfo.csvに、各マテリアルに適用させたい
　シェーダーファイル名を記入することで、マテリアルごとにシェーダーを切り替えることが出来ます。
　※ModelRendererのIsUseMaterialShader関数にtrueを設定することでマテリアルごとのシェーダーが有効になります。

=== シェーダーを作成する際の注意点 ===

2Dテクスチャの受け取り方ルール。
	
2Dテクスチャを受け取る際、"Texture2D tex : register(t0);"のように記述すると思いますが、
tの後ろの番号は以下の表を参照して決めてください
	
テクスチャタイプごとのレジスタ番号表
	マップタイプ	 :	番号
	DiffuseMap		 :	0
	SpecularMap		 :	1
	AmbientMap		 :	2
	EmissiveMap		 :	3
	HeightMap		 :	4
	NormalMap		 :	5
	OpacityMap		 :	6
	ReflectionMap	 :	7
	BaseColor(PBR)	 :	8
	TransmissionMap	 :	9
	Maya_Base		 :	10
	Maya_Specular	 :	11
	Maya_Sp_Color	 :	12
	Maya_Sp_Roughness:	13

PixelShaderのバッファに情報を書き込む際のルール

PSにバッファ情報を渡す場合は、必ずShaderInfoクラスを使用してください。
なお、受け取り側で、
cbuffer Camera : register(b0)
{
    float3 eyePos;
    float camDummy;
}
のように記述すると思いますが、bの後ろの番号は、設定するパラメータを書き込んだ
ShaderInfoのスロット番号と一致させてください。
また、マテリアルごとのPSでバッファを書き込みたい場合は、以下のフォーマットに従ってcsvに記述してください。
PSWriteInfo:{パラメーター名-スロット番号}{パラメーター名-スロット番号}...(任意の数だけ増やす)
		┗この「PSWriteInfo:」は1文字でも間違っていると読みこまれないので注意
	
フレームワークに組み込まれている、スプライトマネージャーの起動コマンド
	(Left_Shift + Space + U_key) + I_key　これらの同時押し
	

=== Transformに関する注意点 ===

SetRotation関数でオイラー角を設定すると、内部でクォータニオンに変換します。
GetRotationは、クォータニオンをオイラー角に変換したものを返します。
これにより、特にジンバルロック境界（Pitch 90度付近）を越えると、設定した値とは異なる値が返されることがあります。
連続的な回転にはRotate関数を使用してください。

