/*+===================================================================
	File: SpriteManagerInspector.hpp
	Summary: スプライトマネージャーのインスペクタービューウィンドウ
	Author: AT13C192 01 青木雄一郎
	Date: 2025/07/26 Sat AM 11:37:51 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "SpriteManagerWindow.hpp"

// ==============================
//  前方宣言
// ==============================
class GameObject;

/// <summary>
/// SpriteManagerInspectorクラス
/// </summary>
class SpriteManagerInspector : public SpriteManagerWindow
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SpriteManagerInspector();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SpriteManagerInspector();

	void Draw(_In_opt_ GameObject *In_2DSprite, _In_opt_ GameObject *In_3DSprite) noexcept;

private:
	DirectX::XMFLOAT3 m_Pos2D;
	DirectX::XMFLOAT3 m_Scale2D;
	DirectX::XMFLOAT3 m_Rotation2D;
	int m_Layer2D;
	char m_cFilePath2D[cx_MaxStringLength]; // ファイルパス

	DirectX::XMFLOAT3 m_Pos3D;
	DirectX::XMFLOAT3 m_Scale3D;
	DirectX::XMFLOAT3 m_Rotation3D;
	int m_Layer3D;
	bool m_bIsBillBoard; // ビルボードスプライトかどうか
	char m_cFilePath3D[cx_MaxStringLength]; // ファイルパス
};
