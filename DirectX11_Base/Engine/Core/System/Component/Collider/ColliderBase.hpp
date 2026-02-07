/*+===================================================================
	File: ColliderBase.hpp
	Summary: 当たり判定のベースクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/9/15 Mon PM 05:06:21 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Core/System/Component/Component.hpp"
#include "Core/DirectX11/System/DX11_Math.hpp"

// ==============================
//  前方宣言
// ==============================
class TreeData;
class CollisionManager;

// ==============================
//  列挙型定義
// ==============================
enum ColliderType
{
	COLLIDER_NONE = -1, // 無効
	COLLIDER_SPHERE, // 球
	COLLIDER_BOX,   // 箱
	COLLIDER_CAPSULE, // カプセル


	COLLIDER_MAX,   // 最大値
};

/// <summary>
/// ColliderBaseクラス
/// </summary>
class ColliderBase : public Component
{
public:
	ColliderBase(_In_ std::string_view In_Name);
	virtual ~ColliderBase();

	virtual void SaveLoad(_In_ DataAccessor *In_Data) override;

	void Init() noexcept override;
	void Update(_In_ float In_DeltaTime) noexcept override;
	void FixedUpdate(_In_ double In_FixedDeltaTime) noexcept override;

	bool CheckCollision(_In_ ColliderBase *In_Other) noexcept;

	virtual void GetAABB(_Out_ DirectX::XMFLOAT3 &Out_LeftTopFront, _Out_ DirectX::XMFLOAT3 &Out_RightBottomBack) const noexcept = 0;

	ColliderType GetType() const noexcept { return m_Type; }

	/// <summary>
	/// 境界ボリュームの中心座標を取得します
	/// </summary>
	/// <returns>境界ボリュームの中心を表すXMFLOAT3ベクトル</returns>
	inline DirectX::XMFLOAT3 GetCenter() const { return m_Center; }
	/// <summary>
	/// 境界ボリュームの中心座標を設定します
	/// </summary>
	/// <param name="[In_Center]">設定する中心座標を表すXMFLOAT3ベクトル</param>
	inline void SetCenter(_In_ const DirectX::XMFLOAT3 &In_Center) { m_Center = In_Center; }

	const bool &IsTrigger() const noexcept { return m_IsTrigger; }
	/// <summary>
	/// 衝突が発生しているかどうかを取得する。
	/// </summary>
	/// <returns>衝突状態を示す真偽値への参照。</returns>
	const bool &IsCollision() const noexcept { return m_IsCollision; }

	void SetIsTrigger(_In_ const bool &In_IsTrigger) noexcept { m_IsTrigger = In_IsTrigger; }

	void SetIsCollision(_In_ const bool &In_IsCollision) noexcept { m_IsCollision = In_IsCollision; }

	TreeData *GetTreeData() noexcept { return m_pTreeData.get(); }

	// 当たり判定呼び出し用
	void CallOnEnter(_In_ ColliderBase *In_Other) noexcept;
	void CallOnStay(_In_ ColliderBase *In_Other) noexcept;
	void CallOnExit(_In_ ColliderBase *In_Other) noexcept;

	// ギズモ描画
	void DrawGizmos(_In_ Gizmos *In_Gizmos) noexcept override final;

protected:

	// 継承先で使用する関数
	virtual void DrawColliderOutline(_In_ Gizmos *In_Gizmos) noexcept = 0;
	virtual bool IsCollisionToBox(_In_ ColliderBase *In_Other) noexcept = 0;
	virtual bool IsCollisionToSphere(_In_ ColliderBase *In_Other) noexcept = 0;
	virtual bool IsCollisionToCapsule(_In_ ColliderBase *In_Other) noexcept = 0;

#ifdef _DEBUG
	virtual void Inspector(_In_ ItemGroup *In_pGroup) override;
#endif // _DEBUG

protected:
	std::shared_ptr<TreeData> m_pTreeData; // オクツリー用データ
	ColliderType m_Type; // コライダーの形状
	bool m_IsTrigger; // トリガーかどうか
	bool m_IsCollision; // 衝突しているかどうか
	DirectX::XMFLOAT3 m_Center; // ローカル座標系での中心位置
private:
	CollisionManager &m_CollisionManager;
};
