/*+===================================================================
	File: SpriteManager.hpp
	Summary: スプライトマネージャークラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/07/14 Mon AM 09:55:16 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "System/ImGui/imgui.h"
#include "System/ImGui/imgui_impl_dx11.h"
#include "System/ImGui/imgui_impl_win32.h"
#include "SpriteManagerWindow.hpp"

// ==============================
//  前方宣言
// ==============================
class Camera;

// ==============================
//  マクロ
// ==============================
#define MENU_OPEN_KEY (Input::IsKeyPress(VK_LSHIFT) && Input::IsKeyPress(VK_SPACE) && Input::IsKeyPress('U') && Input::IsKeyTrigger('I'))

/// <summary>
/// SpriteManagerクラス
/// </summary>
class SpriteManager : public Singleton<SpriteManager>
{
	friend class Singleton<SpriteManager>;
	using SpriteObjects = std::list<GameObject *>;

public:

	enum RenderMode
	{
		_2D = 0,	// 2D描画
		_3D,		// 3D描画

		_MAX_RENDER_MODE
	};

	void Init() noexcept;

	/// <summary>
	/// カメラのビュー行列を設定します。
	/// </summary>
	/// <param name="[In_View]">設定するビュー行列（DirectX::XMFLOAT4X4 型の定数参照）。</param>
	void SetCameraView(_In_ const DirectX::XMFLOAT4X4 &In_View) noexcept { m_View = In_View; }
	/// <summary>
	/// 2Dカメラの射影行列を設定します。
	/// </summary>
	/// <param name="[In_Projection]">設定する2D射影行列（DirectX::XMFLOAT4X4型）の参照。</param>
	void SetCameraProjection2D(_In_ const DirectX::XMFLOAT4X4 &In_Projection) noexcept { m_Projection2D = In_Projection; }
	/// <summary>
	/// 3Dカメラの射影行列を設定します。
	/// </summary>
	/// <param name="[In_Projection]">設定するDirectX::XMFLOAT4X4型の射影行列。</param>
	void SetCameraProjection3D(_In_ const DirectX::XMFLOAT4X4 &In_Projection) noexcept { m_Projection3D = In_Projection; }

	/// <summary>
	/// 状態を更新します。
	/// </summary>
	void Update() noexcept;

	void Draw() noexcept;

	/// <summary>
	/// 操作中ディレクトリの2Dスプライトリストを取得します。
	/// </summary>
	/// <returns>スプライト（Sprite 型ポインタ）のリストへの定数参照。</returns>
	std::list<GameObject *> Get2DSprites() noexcept;

	std::list<GameObject *> Get3DSprites() noexcept;

	/// <summary>
	/// 操作中のディレクトリにあるスプライトを取得します。
	/// </summary>
	/// <param name="[In_SpriteName]">スプライト名</param>
	/// <returns>スプライトへのポインタ</returns>
	GameObject *GetSprite(_In_ const std::string_view &In_SpriteName) const noexcept;

	/// <summary>
	/// スプライトを作成し、操作中のディレクトリに追加します。
	/// </summary>
	/// <param name="[In_SpriteName]">スプライト名</param>
	/// <param name="[In_FilePath]">スプライト</param>
	/// <param name="[In_Is3D]">3Dモードかどうか</param>
	/// <param name="[In_IsBillBoard]">ビルボードかどうか</param>
	/// <param name="[In_Layer]">レイヤー</param>
	/// <param name="[In_Scale]">スケール</param>
	/// <returns>追加されたスプライトへのポインタ。</returns>
	GameObject* CreateSprite(_In_ const std::string_view &In_SpriteName, _In_ const FilePath &In_FilePath,_In_ const bool &In_Is3D, _In_ const bool &In_IsBillBoard = false,_In_ const int &In_Layer = 0, _In_ const float &In_Scale = 1.0f) noexcept;

	/// <summary>
	/// スプライト名が指定されたレンダーモードで競合しているかどうかを判定します。
	/// </summary>
	/// <param name="[In_SpriteName]">確認するスプライト名。</param>
	/// <param name="[In_Mode]">スプライト名の競合を確認するレンダーモード。</param>
	/// <returns>スプライト名が競合している場合は true、そうでない場合は false を返します。</returns>
	bool CheckConflictSpriteName(_In_ const std::string_view &In_SpriteName, _In_ const RenderMode &In_Mode) const noexcept;

	/// <summary>
	/// 指定されたスプライト名のスプライトを削除します。
	/// </summary>
	/// <param name="[In_SpriteName]">削除するスプライトの名前を表す文字列ビュー。</param>
	void DeleteSprite(_In_ const std::string_view &In_SpriteName) noexcept;

	/// <summary>
	/// 全てのスプライトオブジェクトを解放、削除する。
	/// </summary>
	void DeleteAll() noexcept;

	/// <summary>
	/// ImGuiの描画
	/// </summary>
	void DrawImGui() noexcept;

	/// <summary>
	/// 指定されたシーン名で新しいシーンを作成します。
	/// </summary>
	/// <param name="[In_SceneName]">作成するシーンの名前を表す文字列ビュー。</param>
	void CreateScene(_In_ const std::string_view &In_SceneName) noexcept;

	/// <summary>
	/// シーンを変更します。
	/// </summary>
	/// <param name="[In_Index]">変更先のシーンのインデックス。省略時は -1 です。</param>
	void ChangeScene(_In_ const int &In_Index = -1) noexcept;
	
	/// <summary>
	/// 現在選択されている3Dスプライトを2Dスプライトに変換します。
	/// </summary>
	void ConvertTo2D() noexcept;

	/// <summary>
	/// 現在選択されている2Dスプライトを3Dスプライトに変換します。
	/// </summary>
	void ConvertTo3D() noexcept;

	/// <summary>
	/// カメラコンポーネントを設定します。
	/// </summary>
	/// <param name="[In_Camera]">設定するカメラコンポーネントへのポインタ。</param>
	void SetCamera(_In_ Camera *In_Camera) noexcept { m_pCamera = In_Camera; }

	/// <summary>
	/// カメラオブジェクトを設定します。
	/// </summary>
	/// <param name="In_CameraObj">設定するカメラオブジェクトへのポインタ。</param>
	void SetCameraObject(_In_ GameObject *In_CameraObj) noexcept { m_pCameraObj = In_CameraObj; }

	/// <summary>
	/// ビュー行列と射影行列を更新します。
	/// </summary>
	void UpdateViewAndProjection() noexcept;

private:
	SpriteManager();
	~SpriteManager();

	/// <summary>
	/// マネージャウィンドウを初期化します。
	/// </summary>
	void InitManagerWindow() noexcept;

	/// <summary>
	/// スプライトを保存します。
	/// </summary>
	void SaveSprites() const noexcept;

	/// <summary>
	/// スプライトを読み込みます。
	/// </summary>
	void LoadSprites() noexcept;

	/// <summary>
	/// シーンを保存します。
	/// </summary>
	void SaveScene() const noexcept;

	/// <summary>
	/// すべてのシーンを読み込みます。
	/// </summary>
	void LoadAllScene() noexcept;

	/// <summary>
	/// スプライトをマウス制御する処理
	/// </summary>
	void SpriteController() noexcept;

	/// <summary>
	/// 2Dスプライトにカーソルが当たったときの処理
	/// </summary>
	void CursorHit2DSprite() noexcept;

	/// <summary>
	/// 2Dスプライトをマウスで操作するための処理
	/// </summary>
	void MouseControl2DSprite() noexcept;

	/// <summary>
	/// 3Dスプライトにカーソルが当たったときの処理
	/// </summary>
	void CursorHit3DSprite() noexcept;

	/// <summary>
	/// 3Dスプライトをマウスで操作するための処理
	/// </summary>
	void MouseControl3DSprite() noexcept;

	/// <summary>
	/// レイが3Dスプライトにヒットしたかどうかを判定します。
	/// </summary>
	/// <param name="[In_vRayPos]">レイの開始位置を表すDirectX::XMVECTOR型のベクトル。</param>
	/// <param name="[In_vRayDir]">レイの方向を表すDirectX::XMVECTOR型のベクトル。</param>
	void SerchHitRay3DSprite(_In_ DirectX::XMVECTOR In_vRayPos, _In_ DirectX::XMVECTOR In_vRayDir) noexcept;

private:
	SpriteObjects m_SpriteObjects[_MAX_RENDER_MODE]; // スプライトオブジェクトのマップ
	std::list<std::string> m_SpriteNames[_MAX_RENDER_MODE];

	//Sprites m_Sprites[_MAX_RENDER_MODE]; // スプライトのマップ
	//std::list<Sprite *> m_SpritePointerList[_MAX_RENDER_MODE]; // スプライトのポインタリスト
	
	// シーンセーブデータ
	std::map<std::string, FilePathHold> m_SceneSaveData;	// シーン名とパスのマップ
	std::string m_CurrentSceneName;							// 現在のシーン名
	int m_CurrentSceneIndex;								// 現在のシーンのインデックス

	DirectX::XMFLOAT4X4 m_View;				// ビュー行列
	DirectX::XMFLOAT4X4 m_Projection3D;		// 投影行列
	DirectX::XMFLOAT4X4 m_Projection2D;		// 2D投影行列
	DirectX::XMFLOAT4X4 m_BillBoardView;	// ビルボード用のビュー行列

	// 選択中のスプライトのインデックス
	int m_Selected2DSpriteIndex;			// 選択中の2Dスプライトのインデックス
	int m_Selected3DSpriteIndex;			// 選択中の3Dスプライトのインデックス
	float m_ClickPointOffsetX_2D;			// クリックしたXポイントのオフセット
	float m_ClickPointOffsetY_2D;			// クリックしたYポイントのオフセット
	float m_ClickPointOffsetX_3D;			// クリックしたXポイントのオフセット(3D)
	float m_ClickPointOffsetY_3D;			// クリックしたYポイントのオフセット(3D)
	float m_fRayLength;						// レイの長さ
	bool m_bIsLeftClickTrigger;				// 左クリックがトリガーされたかどうか
	bool m_bIs3KeyTrigger;					// 3キーがトリガーされたかどうか

	// --- ImGui ---
	bool m_bIsOpen;									// スプライトマネージャーウィンドウが開いているかどうか
	std::vector<SpriteManagerWindow*> m_vecWindow;	// スプライトマネージャーウィンドウのリスト
	int m_2DIndex;
	int m_3DIndex;

	// カメラコンポーネントへのポインタ
	Camera *m_pCamera;

	// カメラオブジェクトへのポインタ
	GameObject *m_pCameraObj;
};
