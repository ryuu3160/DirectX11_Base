/*+===================================================================
	File: Transform.hpp
	Summary: Transformクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/23 Tue PM 06:37:41 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Core/System/Component/Component.hpp"

/// <summary>
/// Transformクラス
/// </summary>
class Transform : public Component
{
public:
	Transform();
	~Transform();

	virtual void Init() noexcept override;

	void SaveLoad(_In_ DataAccessor *In_Data) final;

	// ------------------------------
	//  親子関係
	// ------------------------------

	/// <summary>
	/// 自身の親を設定します
	/// </summary>
	/// <param name="[In_Parent]">親Transformポインタ</param>
	/// <param name="[In_IsWorldPositionStays]">ワールド座標を維持するかどうか</param>
	void SetParent(_In_opt_ Transform *In_Parent, _In_ bool In_IsWorldPositionStays = true);
	/// <summary>
	/// 自身の親を取得します
	/// </summary>
	/// <returns>親Transformポインタ</returns>
	inline Transform *GetParent() const { return m_pParent; }

	/// <summary>
	/// ルートTransformを取得します
	/// </summary>
	/// <returns>ルートTransformへのポインタ</returns>
	Transform *GetRoot();

	/// <summary>
	/// 子要素の数を取得します
	/// </summary>
	/// <returns>子要素の数</returns>
	int GetChildCount() const { return static_cast<int>(m_Children.size()); }
	/// <summary>
	/// 指定されたインデックスの子トランスフォームを取得します
	/// </summary>
	/// <param name="[In_Index]">取得する子トランスフォームのインデックス</param>
	/// <returns>指定されたインデックスの子トランスフォームへのポインタ</returns>
	Transform *GetChild(_In_ int In_Index) const;

	/// <summary>
	/// 指定された名前のTransformを検索します
	/// </summary>
	/// <param name="[In_Name]">検索するTransformの名前</param>
	/// <returns>見つかったTransformへのポインタ(見つからない場合はnullptr)</returns>
	Transform *FindChild(_In_ std::string_view In_Name);

	/// <summary>
	/// 子トランスフォームのリストを取得します
	/// </summary>
	/// <returns>子トランスフォームへのポインタを含むベクターへの定数参照</returns>
	const std::vector<Transform *> &GetChildren() const;

	/// <summary>
	/// 指定したTransformの子であるかどうかを確認します
	/// </summary>
	/// <param name="[In_Transform]">チェックするトランスフォームへのポインタ</param>
	/// <returns>指定されたトランスフォームが子である場合はtrue、それ以外の場合はfalse</returns>
	bool IsChildOf(_In_ Transform *In_Transform) const noexcept;

	/// <summary>
	/// すべての子を切り離します
	/// </summary>
	void DetachChildren() noexcept;

	// ------------------------------
	//  Transform操作
	// ------------------------------

	/// <summary>
	/// 指定されたベクトルによる平行移動を適用します
	/// </summary>
	/// <param name="[In_Translate]">平行移動のオフセットを表す3Dベクトル</param>
	void Translate(_In_ const DirectX::XMFLOAT3 &In_Translate) noexcept;

	/// <summary>
	/// <para>指定された角度で回転を適用します</para>
	/// <para>連続した回転を行う場合はこちらを使用してください</para>
	/// </summary>
	/// <param name="[In_Angle]">回転角度を表す3次元ベクトル(Degree)</param>
	void Rotate(_In_ float In_PitchDeg, _In_ float In_YawDeg, _In_ float In_RollDeg) noexcept;

	/// <summary>
	/// 指定した点を中心に、指定した軸の周りで指定した角度だけ回転します。
	/// </summary>
	/// <param name="[In_Point]">回転の中心点。</param>
	/// <param name="[In_Axis]">回転軸のベクトル。</param>
	/// <param name="[In_Angle]">回転角度（ラジアン）。</param>
	void RotateAround(_In_ const DirectX::XMFLOAT3 &In_Point, _In_ const DirectX::XMFLOAT3 &In_Axis, _In_ float In_Angle);

	/// <summary>
	/// 指定されたターゲット位置と上方向ベクトルに基づいて、オブジェクトの向きを設定します。
	/// </summary>
	/// <param name="[In_Target]">注視するターゲットの位置ベクトル。</param>
	/// <param name="[In_Up]">上方向を定義するベクトル。デフォルトは (0, 1, 0) です。</param>
	void LookAt(_In_ const DirectX::XMFLOAT3 &In_Target, _In_ const DirectX::XMFLOAT3 &In_Up = DirectX::XMFLOAT3(0, 1, 0));

	// ------------------------------
	//  Getter
	// ------------------------------

	// ------------------------------
	// 行列取得
	// ------------------------------
	DirectX::XMFLOAT4X4 GetLocalMatrix(_In_ bool In_IsTranspose = true) const noexcept;
	DirectX::XMFLOAT4X4 GetWorldMatrix(_In_ bool In_IsTranspose = true) const noexcept;

	// ------------------------------
	// ローカル座標
	// ------------------------------

	DirectX::XMFLOAT3 GetLocalRotation(_In_ bool In_IsDegree = false) noexcept;

	inline DirectX::XMFLOAT3 GetLocalPosition() const noexcept { return m_Pos; }
	inline DirectX::XMFLOAT4 GetLocalQuat() const noexcept { return m_Quat; }
	inline DirectX::XMFLOAT3 GetLocalScale() const noexcept { return m_Scale; }

	// ------------------------------
	// ワールド座標
	// ------------------------------

	DirectX::XMFLOAT3 GetRotation(_In_ bool In_IsDegree = false) noexcept;

	inline DirectX::XMFLOAT3 GetPosition() const noexcept;
	inline DirectX::XMFLOAT4 GetQuat() const noexcept;
	inline DirectX::XMFLOAT3 GetScale() const noexcept;

	// ------------------------------
	// 方向ベクトル取得
	// ------------------------------

	/// <summary>
	/// 前方ベクトルを取得します
	/// </summary>
	/// <param name="[Is_Normalize]">trueの場合、正規化されたベクトルを返します(デフォルトはtrue)</param>
	/// <returns>オブジェクトの前方方向を表すXMFLOAT3ベクトル</returns>
	DirectX::XMFLOAT3 GetFront(_In_ const bool &Is_Normalize = true) const noexcept;
	/// <summary>
	/// 右方向ベクトルを取得します
	/// </summary>
	/// <returns>右方向を表す3次元ベクトル</returns>
	DirectX::XMFLOAT3 GetRight() const noexcept;
	/// <summary>
	/// 上方向ベクトルを取得します
	/// </summary>
	/// <returns>上方向を表す3次元ベクトル</returns>
	DirectX::XMFLOAT3 GetUp() const noexcept;

	/// <summary>
	/// 方向ベクトルをまとめて取得します
	/// </summary>
	/// <param name="[Out_Front]">前方向ベクトルを受け取るオプションの出力パラメーター</param>
	/// <param name="[Out_Right]">右方向ベクトルを受け取るオプションの出力パラメーター</param>
	/// <param name="[Out_Up]">上方向ベクトルを受け取るオプションの出力パラメーター</param>
	/// <param name="[In_IsNormalize]">ベクトルを正規化するかどうかを指定します</param>
	void GetDirectionVectors(_Out_opt_ DirectX::XMFLOAT3 *Out_Front, _Out_opt_ DirectX::XMFLOAT3 *Out_Right,
		_Out_opt_ DirectX::XMFLOAT3 *Out_Up, _In_ bool In_IsNormalize = true) const noexcept;

	// ------------------------------
	//  Setter
	// ------------------------------

	// ------------------------------
	// ローカル座標設定
	// ------------------------------

	void SetLocalPosition(_In_ const DirectX::XMFLOAT3 &In_Pos) noexcept;

	/// <summary>
	/// ローカル回転を設定します
	/// </summary>
	/// <param name="[In_Rotation]">設定する回転値(ラジアン)</param>
	void SetLocalRotation(_In_ const DirectX::XMFLOAT3 &In_Rotation) noexcept;
	void SetLocalScale(_In_ const DirectX::XMFLOAT3 &In_Scale) noexcept;
	void SetLocalQuat(_In_ const DirectX::XMFLOAT4 &In_Quat) noexcept;

	// ------------------------------
	// ワールド座標設定
	// ------------------------------

	void SetPosition(_In_ const DirectX::XMFLOAT3 &In_Pos) noexcept;

	/// <summary>
	/// 回転を設定します
	/// </summary>
	/// <param name="[In_Rotation]">設定する回転値(Degree)</param>
	void SetRotation(_In_ const DirectX::XMFLOAT3 &In_Rotation) noexcept;
	void SetScale(_In_ const DirectX::XMFLOAT3 &In_Scale) noexcept;
	void SetQuat(_In_ const DirectX::XMFLOAT4 &In_Quat) noexcept;

protected:
#ifdef _DEBUG
	virtual void RegisterDebugInspector(_In_ DebugWindow *In_pWindow) override;
#endif

private:

	/// <summary>
	/// <para>クォータニオンからオイラー角を同期して取得します</para>
	/// <para>インスペクター用</para>
	/// </summary>
	/// <param name="[In_Quat]">同期するクォータニオン</param>
	/// <returns>クォータニオンから計算されたオイラー角 (ピッチ、ヨー、ロール) を表す XMFLOAT3 構造体。</returns>
	DirectX::XMFLOAT3 SyncEulerFromQuat(_In_ DirectX::XMFLOAT4 In_Quat) noexcept;

	/// SyncEulerFromQuatのヘルパー関数
	DirectX::XMFLOAT3 ChoiceBestEuler(_In_ const std::vector<DirectX::XMFLOAT3> In_Condidates) const noexcept;

	// SyncEulerFromQuatのヘルパー関数
	std::vector<DirectX::XMFLOAT3> GetPracticalEulerCandidates(_In_ DirectX::XMFLOAT4 In_Quat) const noexcept;
	// SyncEulerFromQuatのヘルパー関数
	std::vector<DirectX::XMFLOAT3> GenerateGimbalLockCandidates(_In_ float In_Pitch, _In_ float In_Yaw, _In_ float In_Roll, _In_ float In_StepDeg = 15.0f) const noexcept;

	// 子オブジェクト関連の内部処理
	void AddChild(_In_ Transform *In_Child);
	void RemoveChild(_In_ Transform *In_Child);
	Transform *FindRecursive(_In_ Transform *In_Current, _In_ std::string_view In_Name) const;
	Transform *GetRootRecursive(_In_ Transform *In_Current) const;

	// 座標変換ヘルパー
	DirectX::XMFLOAT3 WorldToLocalPosition(_In_ const DirectX::XMFLOAT3 &In_WorldPos) const;
	DirectX::XMFLOAT3 LocalToWorldPosition(_In_ const DirectX::XMFLOAT3 &In_LocalPos) const;
	DirectX::XMFLOAT4 WorldToLocalRotation(_In_ const DirectX::XMFLOAT4 &In_WorldQuat) const;
	DirectX::XMFLOAT4 LocalToWorldRotation(_In_ const DirectX::XMFLOAT4 &In_LocalQuat) const;

	void UpdateWorldMatrix() const noexcept;
	void MarkDirty() noexcept;
	void PropagateTransformChanged() noexcept;

private:
	DirectX::XMFLOAT3	m_Pos;		// 座標
	DirectX::XMFLOAT3	m_Scale;	// 拡縮
	DirectX::XMFLOAT4	m_Quat;		// 回転(クォータニオン)

	// Euler角用
	DirectX::XMFLOAT3	m_Euler;		// 回転(オイラー角)
	DirectX::XMFLOAT3	m_AccumEuler;	// オイラー角の累積

	// ワールド行列キャッシュ
	mutable DirectX::XMFLOAT4X4 m_WorldMatrix;

	// フラグ
	mutable bool m_WorldMatrixDirty;	// ワールド行列が最新でない場合true
	bool m_IsSyncEuler;					// オイラー角とクォータニオンの同期が必要かどうか

	std::string m_ParentName;
	Transform *m_pParent;
	std::vector<Transform *> m_Children;
};
