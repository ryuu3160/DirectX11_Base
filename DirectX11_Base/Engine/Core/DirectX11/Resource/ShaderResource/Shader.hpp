/*+===================================================================
	File: Shader.hpp
	Summary: 佐々木先生のものを参考にして作成
	Author: AT13C 01 青木雄一郎
	Date: 2025/05/19 Mon PM 04:39:21 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Engine/Core/DirectX11/Resource/Texture.hpp"
#include "UnorderedAccessView.hpp"

// シェーダーの基本クラス
class Shader
{
protected:
	enum Kind
	{
		Vertex,
		InstancedVertex,
		Geometry,
		Hull,
		Domain,
		Pixel,
		Compute,
	};
protected:
	Shader(Kind kind);
public:
	virtual ~Shader();
public:
	/// <summary>
	/// 指定されたファイルパスからデータを読み込みます。
	/// </summary>
	/// <param name="[In_FileName]">読み込むファイルのパスを指定します。</param>
	/// <returns>操作の結果を示すHRESULT値を返します。成功の場合はS_OK、失敗の場合はエラーコードが返されます。</returns>
	HRESULT Load(_In_ const FilePath &In_FileName) noexcept;
	// 文字列からシェーダをコンパイル
	HRESULT Compile(_In_ const char *In_pCode);


	/// <summary>
	/// 指定されたスロットにデータバッファを書き込みます。
	/// </summary>
	/// <param name="[In_Slot]">データを書き込む対象のスロット番号。</param>
	/// <param name="[In_pData]">書き込むデータへのポインタ。</param>
	void WriteBuffer(_In_ const UINT &In_Slot, _In_ void *In_pData) noexcept;
	/// <summary>
	/// テクスチャを指定したスロットに設定します。
	/// </summary>
	/// <param name="[In_Slot]">テクスチャを設定するスロット番号。</param>
	/// <param name="[In_Tex]">設定するテクスチャへのポインタ。</param>
	void SetTexture(_In_ const UINT &In_Slot, _In_opt_ Texture *In_Tex) noexcept;

	virtual void Bind(void) = 0;

private:
	/// <summary>
	/// データとサイズを指定して処理を行います。
	/// </summary>
	/// <param name="pData">処理対象となるデータへのポインタ。</param>
	/// <param name="size">データのバイト数。</param>
	/// <returns>処理の結果を示すHRESULT値。成功時はS_OK、失敗時はエラーコードが返されます。</returns>
	HRESULT Make(_In_ void *In_pData, _In_ const UINT &In_Size) noexcept;
protected:
	/// <summary>
	/// シェーダーファイルを読み込んだ後、シェーダーの種類別に処理を行う
	/// </summary>
	/// <param name="pData">シェーダー作成に使用するデータへのポインタ。</param>
	/// <param name="size">データのバイト数。</param>
	/// <returns>操作の成功または失敗を示す HRESULT 値。</returns>
	virtual HRESULT MakeShader(_In_ void *In_pData, _In_ const UINT &In_Size) noexcept = 0;

private:
	Kind m_kind;
protected:
	std::vector<ID3D11Buffer *> m_pBuffers;
	std::vector<ID3D11ShaderResourceView *> m_pTextures;
};

//----------
// 頂点シェーダ
class VertexShader : public Shader
{
public:
	VertexShader();
	~VertexShader();
	void Bind(void);
protected:
	/// <summary>
	/// 頂点シェーダーを作成します。
	/// </summary>
	/// <param name="[In_pData]">シェーダー作成に使用するデータへのポインタ。</param>
	/// <param name="[In_Size]">データのサイズ（バイト単位）。</param>
	/// <returns>操作の成功または失敗を示すHRESULT値。</returns>
	HRESULT MakeShader(_In_ void *In_pData, _In_ const UINT &In_Size) noexcept;

private:
	ComPtr<ID3D11VertexShader> m_pVS;
	ComPtr<ID3D11InputLayout> m_pInputLayout;
};

//----------
// インスタンシング頂点シェーダ
class InstancedVertexShader : public Shader
{
public:
	InstancedVertexShader();
	~InstancedVertexShader();
	void Bind(void);
	/// <summary>
	/// インスタンスのシェーダーリソースビューを設定します。
	/// </summary>
	/// <param name="[In_pSRV]">設定する ID3D11ShaderResourceView の ComPtr。</param>
	void SetInstanceSRV(_In_ ID3D11ShaderResourceView* In_pSRV) noexcept { m_pInstanceSRV = In_pSRV; }
protected:
	/// <summary>
	/// 頂点シェーダーを作成します。
	/// </summary>
	/// <param name="[In_pData]">シェーダー作成に使用するデータへのポインタ。</param>
	/// <param name="[In_Size]">データのサイズ（バイト単位）。</param>
	/// <returns>操作の成功または失敗を示すHRESULT値。</returns>
	HRESULT MakeShader(_In_ void *In_pData, _In_ const UINT &In_Size) noexcept;

private:
	ComPtr<ID3D11VertexShader> m_pVS;
	ComPtr<ID3D11InputLayout> m_pInputLayout;
	ID3D11ShaderResourceView* m_pInstanceSRV;
};

//----------
// ピクセルシェーダ
class PixelShader : public Shader
{
public:
	PixelShader();
	~PixelShader();
	void Bind(void);
protected:
	/// <summary>
	/// ピクセルシェーダーを作成します。
	/// </summary>
	/// <param name="[In_pData]">シェーダー作成に使用するデータへのポインタ。</param>
	/// <param name="[In_Size]">データのサイズ（バイト単位）。</param>
	/// <returns>操作の成功または失敗を示すHRESULT値。</returns>
	HRESULT MakeShader(_In_ void *In_pData, _In_ const UINT &In_Size) noexcept;
private:
	ComPtr<ID3D11PixelShader> m_pPS;
};

//----------
// ハルシェーダー
class HullShader : public Shader
{
public:
	HullShader();
	~HullShader();
	void Bind();
	static void Unbind();
protected:
	/// <summary>
	/// ハルシェーダーを作成します。
	/// </summary>
	/// <param name="[In_pData]">シェーダー作成に使用するデータへのポインタ。</param>
	/// <param name="[In_Size]">データのサイズ（バイト単位）。</param>
	/// <returns>操作の成功または失敗を示すHRESULT値。</returns>
	HRESULT MakeShader(_In_ void *In_pData, _In_ const UINT &In_Size) noexcept;
private:
	ComPtr<ID3D11HullShader> m_pHS;
};

//----------
// ドメインシェーダー
class DomainShader : public Shader
{
public:
	DomainShader();
	~DomainShader();
	void Bind();
	static void Unbind();
protected:
	/// <summary>
	/// ドメインシェーダーを作成します。
	/// </summary>
	/// <param name="[In_pData]">シェーダー作成に使用するデータへのポインタ。</param>
	/// <param name="[In_Size]">データのサイズ（バイト単位）。</param>
	/// <returns>操作の成功または失敗を示すHRESULT値。</returns>
	HRESULT MakeShader(_In_ void *In_pData, _In_ const UINT &In_Size) noexcept;
private:
	ComPtr<ID3D11DomainShader> m_pDS;
};

//----------
// ジオメトリシェーダー
class GeometryShader : public Shader
{
public:
	GeometryShader();
	~GeometryShader();
	void Bind();
	/// <summary>
	/// バインドされているリソースを解除します。
	/// </summary>
	static void Unbind();
protected:
	/// <summary>
	/// ジオメトリシェーダーを作成します。
	/// </summary>
	/// <param name="[In_pData]">シェーダー作成に使用するデータへのポインタ。</param>
	/// <param name="[In_Size]">データのサイズ（バイト単位）。</param>
	/// <returns>操作の成功または失敗を示すHRESULT値。</returns>
	HRESULT MakeShader(_In_ void *In_pData, _In_ const UINT &In_Size) noexcept;
private:
	ComPtr<ID3D11GeometryShader> m_pGS;
};

//----------
// コンピュートシェーダー
class ComputeShader : public Shader
{
public:
	ComputeShader();
	~ComputeShader();
	void Bind();
	/// <summary>
	/// バインドされているリソースや接続を解除します。
	/// </summary>
	static void Unbind();
	/// <summary>
	/// 指定された数のスレッドで、アンオーダードアクセスビューの配列に対してディスパッチ処理を行います。
	/// </summary>
	/// <param name="[In_Num]">ディスパッチするスレッドの数。</param>
	/// <param name="[ppUAV]">UnorderedAccessView オブジェクトへのポインタの配列へのポインタ。</param>
	/// <param name="[In_hread]">各ディスパッチで使用するスレッドの識別子またはインデックス。</param>
	void Dispatch(_In_ UINT In_Num, _In_ UnorderedAccessView **ppUAV, _In_ const UINT &In_hread) noexcept;

protected:
	/// <summary>
	/// コンピュートシェーダーを作成します。
	/// </summary>
	/// <param name="[In_pData]">シェーダー作成に使用するデータへのポインタ。</param>
	/// <param name="[In_Size]">データのサイズ（バイト単位）。</param>
	/// <returns>操作の成功または失敗を示すHRESULT値。</returns>
	HRESULT MakeShader(_In_ void *In_pData, _In_ const UINT &In_Size) noexcept;
private:
	ComPtr<ID3D11ComputeShader> m_pCS;
};

