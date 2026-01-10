/*+===================================================================
	File: CameraManager.hpp
	Summary: 作成されたカメラを管理するクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/28 Sun PM 09:49:09 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "ryuu_lib/Singleton/Singleton.hpp"
// ==============================
//	前方宣言
// ==============================
class GameObject;

/// <summary>
/// CameraManagerクラス
/// </summary>
class CameraManager : public Singleton<CameraManager>
{
	friend class Singleton<CameraManager>;
public:

	/// <summary>
	/// カメラオブジェクトを追加します
	/// </summary>
	/// <param name="[In_pCameraObj]">追加するカメラオブジェクトへのポインタ</param>
	void AddCameraObj(_In_ GameObject *In_pCameraObj) noexcept;

	/// <summary>
	/// カメラオブジェクトを削除します
	/// </summary>
	/// <param name="[In_pCameraObj]">削除するカメラオブジェクトへのポインタ</param>
	void RemoveCameraObj(_In_ GameObject *In_pCameraObj) noexcept;

	/// <summary>
	/// メインカメラのゲームオブジェクトを取得します
	/// </summary>
	/// <returns>メインカメラのゲームオブジェクトへのポインタ</returns>
	GameObject *GetMainCameraObj() const noexcept;

	/// <summary>
	/// 指定されたインデックスのカメラオブジェクトを取得します
	/// </summary>
	/// <param name="[In_Index]">取得するカメラのインデックス</param>
	/// <returns>指定されたインデックスに対応するカメラオブジェクトへのポインタ</returns>
	GameObject *GetCameraObj(_In_ int In_Index) const noexcept;

	/// <summary>
	/// 指定された名前に一致するカメラオブジェクトを取得します
	/// </summary>
	/// <param name="[In_Name]">取得するカメラオブジェクトの名前</param>
	/// <returns>指定された名前に一致するカメラオブジェクトへのポインタ(見つからない場合はnullptr)</returns>
	GameObject *GetCameraObj(_In_ std::string_view In_Name) const noexcept;

	/// <summary>
	/// カメラの数を取得します
	/// </summary>
	/// <returns>カメラの数</returns>
	inline int GetCameraCount() const noexcept { return static_cast<int>(m_Cameras.size()); }

private:
	CameraManager();
	~CameraManager();

private:
	std::vector<GameObject *> m_Cameras;
};
