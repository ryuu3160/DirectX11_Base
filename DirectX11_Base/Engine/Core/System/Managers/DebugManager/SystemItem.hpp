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
class Component;
class Transform;
class Texture;

// ==============================
// ItemHierarchy クラス
// ==============================

/// <summary>
/// ヒエラルキーウィンドウ
/// ツリー構造とドラッグ&ドロップをサポート
/// </summary>
class ItemHierarchy : public DebugItem
{
public:
    using SelectCallback = std::function<void(GameObject *)>;

    ItemHierarchy(_In_ std::string_view In_Name, _In_ SceneBase *In_pScene, _In_ SelectCallback In_Func = nullptr);
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

    /// <summary>
    /// 選択されたオブジェクトを注視します
    /// </summary>
    void LookAtSelectedObject();

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
	GameObject *m_pEditorCamera;            // エディターカメラ
    GameObject *m_SelectedObject;           // 選択中のオブジェクト
    GameObject *m_DraggedObject;            // ドラッグ中のオブジェクト
    SelectCallback m_SelectCallback;        // 選択時のコールバック
	std::vector<PendingParentChange> m_PendingParentChanges; // 保留中の親子関係変更

    GameObject *m_RenamingObject;           // リネーム中のオブジェクト
    char m_RenameBuffer[256];               // リネーム用バッファ
    bool m_RenameJustStarted;               // リネーム開始フラグ
};

/// <summary>
/// Unity風のAddComponentセレクター
/// </summary>
class ItemComponentSelector : public DebugItem
{
public:
    using UpdateCallback = std::function<void(GameObject *)>;

    ItemComponentSelector(_In_ std::string_view In_Name, _In_ GameObject *In_pGameObject);
    ~ItemComponentSelector();

    void DrawImGui() override;

    /// <summary>
    /// 対象 GameObject を設定
    /// </summary>
    void SetGameObject(_In_ GameObject *In_pGameObject);

	void ChangeImGuiItem() override;

private:
    void DrawComponentPopup();
    void DrawCategoryTabs();
    void DrawComponentList(const std::string &category);

private:
    GameObject *m_pGameObject;
    char m_SearchBuffer[256];
    std::string m_SelectedCategory;
    bool m_IsAddingComponent;
};

/// <summary>
/// コンポーネント専用のグループアイテム
/// 右クリックメニュー付き
/// </summary>
class ItemComponentGroup : public ItemGroup
{
public:
    ItemComponentGroup(_In_ std::string_view In_Name, _In_ Component *In_pComponent);
    ~ItemComponentGroup();

	void SetIsDeletable(_In_ bool In_IsDeletable) { m_IsDeletable = In_IsDeletable; }
	void SetIsMovable(_In_ bool In_IsMovable) { m_IsMovable = In_IsMovable; }

    void DrawImGui() override;

    void ChangeImGuiItem() override;

private:
    void DrawContextMenu();
    void DeleteComponent();
	void ChangeComponentOrder();

private:
    bool m_IsDeletable;
    bool m_IsMovable;
    Component *m_pComponent;
	std::vector<Component *> m_DeletedComponents;
	std::vector<std::function<void()>> m_MoveActions;
};

/// <summary>
/// Unity のプロジェクトウィンドウ/UE のコンテンツドロワー
/// ファイルブラウザ機能
/// </summary>
class ItemProjectWindow : public DebugItem
{
public:
    ItemProjectWindow(_In_ std::string_view In_Name, _In_ std::string_view In_RootPath);
    ~ItemProjectWindow();

    void DrawImGui() override;

    /// <summary>
    /// ルートパスを設定
    /// </summary>
    void SetRootPath(_In_ std::string_view In_RootPath);

    /// <summary>
    /// 選択中のファイルパスを取得
    /// </summary>
    std::string GetSelectedFilePath() const;

    /// <summary>
    /// ファイルが選択されたときのコールバックを設定
    /// </summary>
    void SetFileSelectedCallback(_In_ std::function<void(const std::string &)> In_Callback);

    /// <summary>
    /// アイコン画像を登録（拡張子とテクスチャIDV のペア）
    /// </summary>
    void RegisterIcon(_In_ std::string_view In_Extension, _In_ std::shared_ptr<Texture> In_Texture);

    /// <summary>
    /// デフォルトフォルダアイコンを設定
    /// </summary>
    void SetDefaultFolderIcon(_In_ std::shared_ptr<Texture> In_Texture);

    /// <summary>
    /// デフォルトファイルアイコンを設定
    /// </summary>
    void SetDefaultFileIcon(_In_ std::shared_ptr<Texture> In_Texture);

	/// <summary>
	/// アイコンのサイズを取得
	/// </summary>
	/// <returns>アイコンのサイズ</returns>
	float GetIconSize() const { return m_IconSize; }

    /// <summary>
    /// グリッドのアイコンサイズを設定
    /// </summary>
    void SetIconSize(_In_ float In_Size) { m_IconSize = In_Size; }

private:
    /// <summary>
    /// フォルダツリーを描画（左側）
    /// </summary>
    void DrawFolderTree(_In_ const std::filesystem::path &In_Path, _In_ bool In_IsRoot = false);

    /// <summary>
    /// ファイル一覧を描画（右側・グリッド表示）
    /// </summary>
    void DrawFileGrid();

    /// <summary>
    /// パンくずリストを描画
    /// </summary>
    void DrawBreadcrumb();

    /// <summary>
    /// ファイルアイコン（テクスチャ）を取得
    /// </summary>
    std::shared_ptr<Texture> GetFileIconTexture(_In_ const std::filesystem::path &In_Path);

    /// <summary>
    /// 現在のフォルダのファイル一覧を更新
    /// </summary>
    void RefreshCurrentFolder();

    /// <summary>
    /// フィルタリング処理
    /// </summary>
    bool PassFilter(_In_ const std::filesystem::path &In_Path) const;

    /// <summary>
    /// 新規フォルダ作成
    /// </summary>
    void CreateNewFolder();

    /// <summary>
    /// ファイル/フォルダ削除
    /// </summary>
    void DeleteItem(_In_ const std::filesystem::path &In_Path);

    /// <summary>
    /// リネーム処理
    /// </summary>
    void RenameItem(_In_ const std::filesystem::path &In_OldPath, _In_ const std::string &In_NewName);

    /// <summary>
    /// ファイルシステムの変更を監視するスレッド
    /// </summary>
    void WatchFileSystemChanges();

    /// <summary>
    /// ファイル監視を開始
    /// </summary>
    void StartWatching();

    /// <summary>
    /// ファイル監視を停止
    /// </summary>
    void StopWatching();

private:
    // ファイル監視関連
    std::thread m_WatcherThread;
    std::atomic<bool> m_IsWatching;
    std::atomic<bool> m_NeedsRefresh;
    HANDLE m_hDirectory;

    std::filesystem::path m_RootPath;
    std::filesystem::path m_CurrentPath;
    std::filesystem::path m_SelectedItem;

    std::vector<std::filesystem::path> m_CurrentFiles;
    std::vector<std::filesystem::path> m_CurrentFolders;

    char m_SearchBuffer[256];
    char m_RenameBuffer[256];

    bool m_IsRenaming;
    std::filesystem::path m_RenamingItem;

    std::function<void(const std::string &)> m_FileSelectedCallback;

    // アイコン関連
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_IconMap;
    std::shared_ptr<Texture> m_DefaultFolderIcon;
    std::shared_ptr<Texture> m_DefaultFileIcon;
    float m_IconSize;

    // フィルター設定
    bool m_ShowImages;
    bool m_ShowModels;
    bool m_ShowScripts;
    bool m_ShowAll;
};
