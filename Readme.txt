このフレームワークはSPの授業で使用しているものを改造して作成したものです

=== 基本事項 ===
・ソリューションファイル(.sln)と同じ階層にある、ProjectSetUp.exeを起動して、任意の名前を入力することで、
　プロジェクト名を変更することが出来ます。

・読みこんだモデル(fbxファイル)と同じ階層に作成される、fbx名_MaterialShaderInfo.csvに、各マテリアルに適用させたい
　シェーダーファイル名を記入することで、マテリアルごとにシェーダーを切り替えることが出来ます。
　※ModelRendererのIsUseMaterialShader関数にtrueを設定することでマテリアルごとのシェーダーが有効になります。

=== シェーダーを作成する際の注意点 ===
テクスチャの受け取り方にルールがあります。
	
テクスチャを受け取る際、"Texture2D tex : register(t0);"のように書くかと思いますが、
tの後ろの番号は以下の表を参照して決めてください
	
テクスチャタイプごとのレジスタ番号
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

	
フレームワークに組み込まれている、スプライトマネージャーの起動コマンド
	(Left_Shift + Space + U_key) + I_key　これらの同時押し

