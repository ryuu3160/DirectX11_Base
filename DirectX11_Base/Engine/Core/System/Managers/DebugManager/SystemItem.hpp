/*+===================================================================
	File: SystemItem.hpp
	Summary: システム側でのみ使用するデバッグ項目
	Author: AT13C192 01 青木雄一郎
	Date: 2026/1/12 Mon PM 08:07:58 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "DebugItem.hpp"

// ===============================
//  前方宣言
// ===============================
class SceneBase;
class GameObject;
class Transform;

// ==============================
// ItemHierarchy クラス
// ==============================

/// <summary>
/// Unity 風のヒエラルキーウィンドウ
/// ツリー構造とドラッグ&ドロップをサポート
/// </summary>
class ItemHierarchy : public DebugItem
{
public:
    using SelectCallback = std::function<void(GameObject *)>;

    ItemHierarchy(_In_ std::string In_Name, _In_ SceneBase *In_pScene, _In_ SelectCallback In_Func = nullptr);
    ~ItemHierarchy();

    void DrawImGui() override;

    /// <summary>
    /// 選択中のオブジェクトを取得
    /// </summary>
    GameObject *GetSelectedObject() const { return m_SelectedObject; }

    /// <summary>
    /// オブジェクトを選択
    /// </summary>
    void SelectObject(_Inout_ GameObject *In_Obj);

private:
    /// <summary>
    /// ルートオブジェクトのリストを描画
    /// </summary>
    void DrawRootObjects();

    /// <summary>
    /// オブジェクトをツリーノードとして描画(再帰)
    /// </summary>
    void DrawObjectNode(_Inout_ GameObject *In_Obj);

    /// <summary>
    /// ドラッグ&ドロップのソースを設定
    /// </summary>
    void HandleDragSource(_In_ GameObject *In_Obj);

    /// <summary>
    /// ドラッグ&ドロップのターゲットを設定
    /// </summary>
    void HandleDropTarget(_In_ GameObject *In_Obj);

    /// <summary>
    /// 右クリックメニューを表示
    /// </summary>
    void ShowContextMenu(_In_ GameObject *In_Obj);

private:

    /// <summary>
    /// リネームを開始
    /// </summary>
    void StartRename(_In_ GameObject *In_Obj);

    /// <summary>
    /// リネーム入力を描画
    /// </summary>
    void DrawRenameInput(_In_ GameObject *In_Obj, _In_ int In_ChildCountBefore);

    /// <summary>
    /// リネームを完了
    /// </summary>
    void FinishRename(_In_ bool In_IsConfirm);

    /// <summary>
    /// 保留中の親の変更を実行します
    /// </summary>
    void ExecutePendingParentChanges();

private:
    struct PendingParentChange
    {
        GameObject *ChildObj;
        Transform *NewParentTransform;
    };

    SceneBase *m_pScene;                    // 対象シーン
    GameObject *m_SelectedObject;           // 選択中のオブジェクト
    GameObject *m_DraggedObject;            // ドラッグ中のオブジェクト
    SelectCallback m_SelectCallback;        // 選択時のコールバック
	std::vector<PendingParentChange> m_PendingParentChanges; // 保留中の親子関係変更

    GameObject *m_RenamingObject;           // リネーム中のオブジェクト
    char m_RenameBuffer[256];               // リネーム用バッファ
    bool m_RenameJustStarted;               // リネーム開始フラグ
};