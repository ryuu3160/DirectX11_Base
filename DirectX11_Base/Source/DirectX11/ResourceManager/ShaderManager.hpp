/*+===================================================================
	File: ShaderManager.hpp
	Summary: シェーダーを管理するクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/08/17 Sun AM 01:33:29 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "DirectX11/Resource/Shaders/Shader.hpp"

/// <summary>
/// ShaderManagerクラス
/// </summary>
class ShaderManager : public Singleton<ShaderManager>
{
	friend class Singleton<ShaderManager>;
public:

	/// <summary>
	/// シェーダーファイルの読み込みとセットアップを行う
	/// </summary>
	/// <param name="[In_FileNames]">読みこむシェーダーファイルの名前</param>
	void SetupShaders(_In_ const std::vector<std::string> &In_FileNames) noexcept;

	/// <summary>
	/// 指定されたファイル名からシェーダーを取得します。
	/// </summary>
	/// <param name="[In_FileName]">取得するシェーダーのファイルパス。</param>
	/// <returns>取得した Shader オブジェクトへのポインタ。</returns>
	Shader *GetShader(_In_ const FilePath &In_FileName) noexcept;

	/// <summary>
	/// 指定されたファイル名のシェーダーを解放します。
	/// </summary>
	/// <param name="[In_FileName]">解放するシェーダーのファイルパス。</param>
	void ReleaseShader(_In_ const FilePath &In_FileName) noexcept;
private:
	ShaderManager();
	~ShaderManager() override;

	/// <summary>
	/// シェーダーを作成する
	/// </summary>
	/// <param name="[In_FileName]">シェーダーファイル名</param>
	/// <returns>作成したシェーダーへのポインタ</returns>
	template<typename T, typename = std::enable_if_t<std::is_base_of<Shader, T>::value>>
	T *CreateShader(_In_ const FilePath &In_FileName)
	{
		// デバッグモードのみ名前の競合チェック
#ifdef _DEBUG
		auto itr = m_mapShaders.find(In_FileName.data());
		if (itr != m_mapShaders.end())
		{
			// 既に同じ名前のシェーダーが存在する場合はnullptrを返す
			std::string msg = "Failed to create shader: " + std::string(In_FileName) + ". \nShader already exists with the same name.";
			MessageBoxA(nullptr, msg.c_str(), "Error", MB_OK);
			return nullptr;
		}
#endif

		// シェーダー作成
		T *shader = new T();
		std::string name = std::string(In_FileName);
		m_mapShaders.insert({ name, shader});

		return shader; // 作成したシェーダーを返す
	}

private:
	std::unordered_map<std::string, Shader *> m_mapShaders; // シェーダーのマップ
};
