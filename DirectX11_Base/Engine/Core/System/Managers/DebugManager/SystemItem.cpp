/*+===================================================================
	File: SystemItem.cpp
	Summary: システム側でのみ使用するデバッグ項目
	Author: AT13C192 01 青木雄一郎
	Date: 2026/1/12 Mon PM 08:08:03 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "SystemItem.hpp"
#include "Core/System/Component/Component.hpp"
#include "Core/System/Managers/DebugManager/DebugManager.hpp"
#include "Core/System/Object/GameObject.hpp"
#include "Core/System/Scene/SceneBase.hpp"
#include "Engine/Core/System/Component/ComponentRegistry.hpp"
#include "Engine/Core/DirectX11/System/DX11_Math.hpp"
// ==============================
//	定数定義
// ==============================
namespace
{
}
#include <thread>
#include <atomic>
#include <mutex>
// ==============================
//  ItemHierarchy
// ==============================

ItemHierarchy::ItemHierarchy(_In_ std::string_view In_Name, _In_ SceneBase *In_pScene, _In_ SelectCallback In_Func)
    : m_pScene(In_pScene)
    , m_SelectedObject(nullptr)
    , m_DraggedObject(nullptr)
    , m_SelectCallback(In_Func)
	, m_RenamingObject(nullptr)
	, m_RenameJustStarted(false)
{
    m_Name = In_Name.data();
    m_Kind = Kind::__Hierarchy;
    m_RenameBuffer[0] = '\0';
	m_pEditorCamera = m_pScene->GetObject("EditorCamera");

    if(!m_pEditorCamera)
		Debug::DebugLogError("ItemHierarchy: EditorCamera Not Found in the scene. Please ensure there is a GameObject named 'EditorCamera' in the scene.");
}

ItemHierarchy::~ItemHierarchy()
{
}

void ItemHierarchy::DrawImGui()
{
    if(m_Kind != Kind::__Hierarchy || !m_pScene)
        return;

    // ルートオブジェクトを描画
    DrawRootObjects();

    // 空白領域をクリックしたら選択解除
    if(ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        if(!ImGui::IsAnyItemHovered())
        {
            m_SelectedObject = nullptr;
            if(m_SelectCallback)
                m_SelectCallback(nullptr);
        }
    }

	// 右クリックメニュー(空白領域)
    if(ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        if(!ImGui::IsAnyItemHovered())
        {
			ImGui::OpenPopup("Hierarchy_EmptyContextMenu");
        }
    }
	if(ImGui::BeginPopup("Hierarchy_EmptyContextMenu"))
    {
        // 空白領域の右クリックメニュー
        if(ImGui::MenuItem("Create Empty GameObject"))
        {
            // 新しい空のGameObjectを作成
            GameObject *NewObj = m_pScene->CreateObject("Empty");
            if(NewObj)
            {
                SelectObject(NewObj);
                StartRename(NewObj);
            }
        }
        ImGui::EndPopup();
    }

    // 予約された親子関係の変更を実行
    ExecutePendingParentChanges();
}

void ItemHierarchy::DrawRootObjects()
{
    if(!m_pScene)
        return;

    // シーン内のすべてのオブジェクトを取得
    const auto &AllObjects = m_pScene->GetHierarchyObjects();

	// オブジェクトをルートから描画
    for(auto obj : AllObjects)
    {
        if(obj && obj->GetTransform()->GetParent() == nullptr)
        {
            DrawObjectNode(obj);
        }
    }
}

void ItemHierarchy::DrawObjectNode(_Inout_ GameObject *In_Obj)
{
    if(!In_Obj)
        return;

    Transform *transform = In_Obj->GetTransform();
    // ドロップ前の子の数を保存
    int ChildCountBefore = transform->GetChildCount();

    // リネーム中の場合はインライン編集
    if(m_RenamingObject == In_Obj)
    {
        DrawRenameInput(In_Obj, ChildCountBefore);
        return;  // リネーム中は通常の TreeNode を表示しない
    }

    // ツリーノードのフラグ
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_OpenOnDoubleClick |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    // 選択状態
    if(m_SelectedObject == In_Obj)
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    // 子がいない場合は葉ノード
    if(transform->GetChildCount() == 0)
    {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    std::string label = In_Obj->GetName();

    // ツリーノードを描画
    bool NodeOpen = ImGui::TreeNodeEx(In_Obj, flags, "%s", label.c_str());

    // ドラッグソースとして設定
    HandleDragSource(In_Obj);

    // ドロップターゲットとして設定
    HandleDropTarget(In_Obj);

    // クリックで選択、ダブルクリックでリネーム
    if(ImGui::IsItemClicked())
    {
        if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && m_SelectedObject == In_Obj)
            StartRename(In_Obj);
        else
            SelectObject(In_Obj);
    }

    // F2キーでリネーム開始
    if(m_SelectedObject == In_Obj && ImGui::IsKeyPressed(ImGuiKey_F2))
    {
        StartRename(In_Obj);
    }

    // Delで選択しているオブジェクトを削除
    if(m_SelectedObject == In_Obj && ImGui::IsKeyPressed(ImGuiKey_Delete))
    {
        // オブジェクトを削除
        In_Obj->DestroySelf();

        m_SelectedObject = nullptr;
        if(m_SelectCallback)
            m_SelectCallback(nullptr);
    }

    // Fキーで選択中のオブジェクトを注視
    if(ImGui::IsKeyPressed(ImGuiKey_F) && ImGui::IsWindowFocused())
        LookAtSelectedObject();

    // 右クリックメニュー
    if(ImGui::BeginPopupContextItem())
    {
        ShowContextMenu(In_Obj);
        ImGui::EndPopup();
    }

    // 子ノードを描画
    if(NodeOpen && ChildCountBefore > 0)
    {
        // 子リストのスナップショット
        std::vector<Transform *> ChildrenSnapshot = transform->GetChildren();
        for(Transform *ChildTransform : ChildrenSnapshot)
        {
            GameObject *ChildObj = ChildTransform->GetGameObject();
            DrawObjectNode(ChildObj);
        }

        ImGui::TreePop();
    }
}

void ItemHierarchy::HandleDragSource(_In_ GameObject *In_Obj)
{
    // ドラッグ開始
    if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
    {
        // ペイロードを設定（GameObject のポインタ）
        ImGui::SetDragDropPayload("HIERARCHY_OBJECT", &In_Obj, sizeof(GameObject *));

        // ドラッグ中の表示
        ImGui::Text("Move:  %s", In_Obj->GetName().c_str());

        m_DraggedObject = In_Obj;

        ImGui::EndDragDropSource();
    }
}

void ItemHierarchy::HandleDropTarget(_In_ GameObject *In_Obj)
{
    // ドロップターゲット
    if(ImGui::BeginDragDropTarget())
    {
        if(const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("HIERARCHY_OBJECT"))
        {
            GameObject *DroppedObj = *static_cast<GameObject **>(payload->Data);

            if(DroppedObj && DroppedObj != In_Obj)
            {
                // 親子関係を設定
                Transform *DroppedTransform = DroppedObj->GetTransform();
                Transform *TargetTransform = In_Obj->GetTransform();

                // 循環参照チェック(objがdroppedObj の子孫でないか)
                bool IsCircular = false;
                Transform *ancestor = TargetTransform;
                while(ancestor != nullptr)
                {
                    if(ancestor == DroppedTransform)
                    {
                        IsCircular = true;
                        break;
                    }
                    ancestor = ancestor->GetParent();
                }

                if(!IsCircular)
                {
                    // 親子関係の変更を予約
                    m_PendingParentChanges.push_back({ DroppedObj,TargetTransform });
                }
            }
        }

        ImGui::EndDragDropTarget();
    }
}

void ItemHierarchy::ShowContextMenu(_In_ GameObject *In_Obj)
{
    if(ImGui::MenuItem("Rename", "F2"))
    {
        StartRename(In_Obj);
    }

    ImGui::Separator();

    if(ImGui::MenuItem("Delete", "Del"))
    {
        // オブジェクトを削除
        In_Obj->DestroySelf();
        if(m_SelectedObject == In_Obj)
        {
            m_SelectedObject = nullptr;
            if(m_SelectCallback)
                m_SelectCallback(nullptr);
        }
    }

    ImGui::Separator();

    if(In_Obj->GetTransform()->GetParent() != nullptr)
    {
        if(ImGui::MenuItem("Detach from Parent"))
        {
            // 親から外す処理の予約
			m_PendingParentChanges.push_back({ In_Obj, nullptr });
        }
    }
}

void ItemHierarchy::LookAtSelectedObject()
{
	if(m_SelectedObject && m_pEditorCamera)
    {
        // カメラのフロントベクトルを取得
        DirectX::XMFLOAT3 Forward = m_pEditorCamera->GetTransform()->GetFront();

        // オブジェクトの位置
        DirectX::XMFLOAT3 TargetPos = m_SelectedObject->GetTransform()->GetPosition();

        // フォーカス距離
        float FocusDistance = 5.0f;

        // 新しいカメラ位置
        DirectX::XMFLOAT3 newCameraPos = TargetPos - Forward * FocusDistance;

        // カメラの位置を更新
        m_pEditorCamera->GetTransform()->SetPosition(newCameraPos);
    }
}

void ItemHierarchy::StartRename(_In_ GameObject *In_Obj)
{
    m_RenamingObject = In_Obj;
    m_RenameJustStarted = true;

    // 現在の名前をバッファにコピー
    strncpy_s(m_RenameBuffer, In_Obj->GetName().c_str(), sizeof(m_RenameBuffer) - 1);
    m_RenameBuffer[sizeof(m_RenameBuffer) - 1] = '\0';
}

void ItemHierarchy::DrawRenameInput(_In_ GameObject *In_Obj, _In_ int In_ChildCountBefore)
{
    // インデントを保持
    ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());

    // InputText でリネーム
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

    // リネーム開始直後にフォーカスを設定
    if(m_RenameJustStarted)
    {
        ImGui::SetKeyboardFocusHere();
        m_RenameJustStarted = false;
    }

    ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
    bool confirmed = ImGui::InputText("##rename", m_RenameBuffer, sizeof(m_RenameBuffer), flags);

    // Enter で確定
    if(confirmed)
    {
        FinishRename(true);
    }
    else if(ImGui::IsKeyPressed(ImGuiKey_Escape)) // Esc でキャンセル
    {
        FinishRename(false);
    }
    else if(ImGui::IsItemDeactivatedAfterEdit()) // フォーカスを失ったら確定
    {
        FinishRename(true);
    }
	else if(m_SelectedObject != In_Obj && !m_RenameJustStarted) // 変更されていない&選択解除されたらキャンセル
    {
        FinishRename(false);
    }

    ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());

    // 子ノードも表示(折りたたみはできない)
    if(In_ChildCountBefore > 0)
    {
        ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());

        std::vector<Transform *> childrenSnapshot = In_Obj->GetTransform()->GetChildren();
        for(Transform *childTransform : childrenSnapshot)
        {
            GameObject *childObj = childTransform->GetGameObject();
            DrawObjectNode(childObj);
        }

        ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
    }
}

void ItemHierarchy::FinishRename(_In_ bool In_IsConfirm)
{
    if(In_IsConfirm && m_RenamingObject)
    {
        std::string newName = m_RenameBuffer;

        // バリデーション
        if(newName.empty())
        {
            DebugManager::GetInstance().DebugLogWarning("Name cannot be empty");
        }
        else if(newName != m_RenamingObject->GetName())
        {
            // 名前の変更
			m_RenamingObject->Rename(newName);
			SelectObject(m_RenamingObject);
        }
    }

    m_RenamingObject = nullptr;
    m_RenameBuffer[0] = '\0';
}

void ItemHierarchy::ExecutePendingParentChanges()
{
    for(auto &itr : m_PendingParentChanges)
    {
		itr.ChildObj->GetTransform()->SetParent(itr.NewParentTransform);
    }
    m_PendingParentChanges.clear();
}

void ItemHierarchy::SelectObject(_Inout_ GameObject *In_Obj)
{
    m_SelectedObject = In_Obj;
    if(m_SelectCallback)
    {
        m_SelectCallback(In_Obj);
    }
}

// ==============================
//  ItemComponentSelector
// ==============================

ItemComponentSelector::ItemComponentSelector(_In_ std::string_view In_Name, _In_ GameObject *In_pGameObject)
	: m_pGameObject(In_pGameObject), m_IsAddingComponent(false)
    , m_SelectedCategory("All")
{
    m_Name = In_Name.data();
    m_Kind = Kind::Command;
    m_SearchBuffer[0] = '\0';
}

ItemComponentSelector::~ItemComponentSelector()
{
}

void ItemComponentSelector::DrawImGui()
{
    if(!m_pGameObject)
        return;

    // AddComponentボタン
    float buttonWidth = ImGui::GetContentRegionAvail().x;
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.5f, 0.15f, 1.0f));

    if(ImGui::Button("+ Add Component", ImVec2(buttonWidth, 35)))
    {
        ImGui::OpenPopup("AddComponentPopup");
        m_SearchBuffer[0] = '\0';
    }

    ImGui::PopStyleColor(3);

    // ポップアップ
    DrawComponentPopup();
}

void ItemComponentSelector::SetGameObject(_In_ GameObject *In_pGameObject)
{
    m_pGameObject = In_pGameObject;
}

void ItemComponentSelector::ChangeImGuiItem()
{
    if(m_IsAddingComponent)
    {
        // インスペクターを更新
        m_pGameObject->ReloadingInspector();
		m_IsAddingComponent = false;
    }
}

void ItemComponentSelector::DrawComponentPopup()
{
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);

    if(ImGui::BeginPopup("AddComponentPopup"))
    {
        ImGui::TextColored(ImVec4(0.3f, 0.8f, 1.0f, 1.0f), "Add Component");
        ImGui::SameLine();

        auto &registry = ComponentRegistry::GetInstance();
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "(%zu available)", registry.GetComponentCount());

        ImGui::Separator();

		// 検索ボックス
        ImGui::SetNextItemWidth(-1);
        ImGui::InputTextWithHint("##ComponentSearch", "Search...", m_SearchBuffer, sizeof(m_SearchBuffer));

        ImGui::Spacing();
        DrawCategoryTabs();

        ImGui::Spacing();

        if(ImGui::Button("Close", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void ItemComponentSelector::DrawCategoryTabs()
{
    auto &registry = ComponentRegistry::GetInstance();
    auto categories = registry.GetAllCategories();

    // Allカテゴリを先頭に追加
    categories.insert(categories.begin(), "All");

    if(ImGui::BeginTabBar("ComponentCategories"))
    {
        for(const auto &category : categories)
        {
            if(ImGui::BeginTabItem(category.c_str()))
            {
                m_SelectedCategory = category;
                DrawComponentList(category);
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }
}

void ItemComponentSelector::DrawComponentList(const std::string &category)
{
    auto &registry = ComponentRegistry::GetInstance();

    // 検索文字列を小文字に変換
    std::string searchStr = m_SearchBuffer;
    std::transform(searchStr.begin(), searchStr.end(), searchStr.begin(), ::tolower);

    ImGui::BeginChild("ComponentListChild", ImVec2(0, 250), true);

    for(const auto &[name, info] : registry.GetAllComponents())
    {
        // カテゴリフィルター
        if(category != "All" && info.Category != category)
            continue;

        // 検索フィルター
        if(!searchStr.empty())
        {
            std::string lowerName = info.Name;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

            std::string lowerDesc = info.Description;
            std::transform(lowerDesc.begin(), lowerDesc.end(), lowerDesc.begin(), ::tolower);

            if(lowerName.find(searchStr) == std::string::npos &&
                lowerDesc.find(searchStr) == std::string::npos)
                continue;
        }

        // コンポーネント項目
        ImGui::PushID(info.Name.c_str());

        if(ImGui::Selectable(info.Name.c_str(), false, ImGuiSelectableFlags_None, ImVec2(0, 30)))
        {
            // コンポーネントを追加
            if(m_pGameObject)
            {
                Component *newComponent = m_pGameObject->AddComponentByName(info.Name);

                if(newComponent)
                    m_IsAddingComponent = true;
            }

            ImGui::CloseCurrentPopup();
        }

        // ツールチップ（ホバー時に説明を表示）
        if(ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(300.0f);

            // コンポーネント名
            ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "%s", info.Name.c_str());
            ImGui::Separator();

            // 説明
			std::string Description = Util::ShiftJISToUTF8(info.Description);
            ImGui::TextWrapped("%s", Description.c_str());

            ImGui::Spacing();

            // カテゴリ
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Category: %s", info.Category.c_str());

            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }

        ImGui::PopID();
    }

    ImGui::EndChild();
}

// ==============================
//  ItemComponentGroup
// ==============================

ItemComponentGroup::ItemComponentGroup(_In_ std::string_view In_Name, _In_ Component *In_pComponent)
    : ItemGroup(In_Name)
    , m_pComponent(In_pComponent)
	, m_IsDeletable(true), m_IsMovable(true)
{
    m_Kind = Kind::Group;
    m_DeletedComponents.clear();
    m_MoveActions.clear();
}

ItemComponentGroup::~ItemComponentGroup()
{
}

void ItemComponentGroup::DrawImGui()
{
    if(m_Items.empty())
        return;

    // CollapsingHeader を表示
    bool isOpen = ImGui::CollapsingHeader(m_Name.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

    // 右クリックメニュー
    if(ImGui::BeginPopupContextItem())
    {
        DrawContextMenu(); 
        ImGui::EndPopup();
    }

    // グループが展開されていれば中身を表示
    if(isOpen)
    {
        for(auto &itr : m_Items)
        {
            itr->DrawImGui();
        }
    }
}

void ItemComponentGroup::ChangeImGuiItem()
{
    // 順序変更の処理
    ChangeComponentOrder();

    // 削除されたコンポーネントの処理
    DeleteComponent();
}

void ItemComponentGroup::DrawContextMenu()
{
    if(!m_pComponent)
        return;

    // ===========================
    // Remove Component
    // ===========================
    if(m_IsDeletable)
    {
        if(ImGui::MenuItem("Remove Component"))
        {
			m_DeletedComponents.push_back(m_pComponent);
        }
        ImGui::Separator();
    }

    // ===========================
    // Move Up/Down
    // ===========================

    if(m_IsMovable)
    {
        if(ImGui::MenuItem("Move Up"))
        {
            m_MoveActions.push_back([this]()
            {
                m_pComponent->ChangeOrderUP();
            });
        }

        if(ImGui::MenuItem("Move Down"))
        {
            m_MoveActions.push_back([this]()
            {
                m_pComponent->ChangeOrderDown();
            });
        }
    }
}

void ItemComponentGroup::DeleteComponent()
{
    if(m_DeletedComponents.empty())
        return;

    auto ComponentsToDelete = std::move(m_DeletedComponents);
    GameObject *pGameObj = m_pComponent->GetGameObject();

    m_DeletedComponents.clear();

    for(auto &itr : ComponentsToDelete)
    {
        if(itr)
            itr->DestroySelf();
    }
    // インスペクターを更新
    if(pGameObj)
        pGameObj->ReloadingInspector();
}

void ItemComponentGroup::ChangeComponentOrder()
{
    if(m_MoveActions.empty())
        return;

	auto MoveActionsToExecute = std::move(m_MoveActions);
	GameObject *pGameObj = m_pComponent->GetGameObject();

	m_MoveActions.clear();

    for(auto &itr : MoveActionsToExecute)
    {
        if(itr)
            itr();
    }
    // インスペクターを更新
    if(pGameObj)
        pGameObj->ReloadingInspector();
}

// ==============================
//  ItemProjectWindow
// ==============================

ItemProjectWindow::ItemProjectWindow(_In_ std::string_view In_Name, _In_ std::string_view In_RootPath)
    : m_RootPath(In_RootPath)
    , m_CurrentPath(In_RootPath)
    , m_IsRenaming(false)
    , m_ShowImages(true)
    , m_ShowModels(true)
    , m_ShowScripts(true)
    , m_ShowAll(true)
    , m_DefaultFolderIcon(nullptr)
    , m_DefaultFileIcon(nullptr)
    , m_IconSize(80.0f)
    , m_IsWatching(false)
    , m_NeedsRefresh(false)
    , m_hDirectory(INVALID_HANDLE_VALUE)
    , m_hStopEvent(nullptr)
    , m_ShowDeleteConfirmation(false)
{
    m_Name = In_Name.data();
    m_Kind = Kind::__ProjectWindow;
    m_SearchBuffer[0] = '\0';
    m_RenameBuffer[0] = '\0';

    // 停止イベント
    m_hStopEvent = CreateEventA(nullptr, TRUE, FALSE, nullptr);

    ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));
    m_Overlapped.hEvent = CreateEventA(nullptr, TRUE, FALSE, nullptr);

    // ルートパスが存在しない場合は作成
    if(!std::filesystem::exists(m_RootPath))
    {
        std::filesystem::create_directories(m_RootPath);
    }

    RefreshCurrentFolder();
	// ディレクトリの監視を開始
    StartWatching();
}

ItemProjectWindow::~ItemProjectWindow()
{
	// 監視を停止
    StopWatching();

    // イベントハンドルを閉じる
    if(m_Overlapped.hEvent != nullptr)
    {
        CloseHandle(m_Overlapped.hEvent);
        m_Overlapped.hEvent = nullptr;
    }

    if(m_hStopEvent != nullptr)
    {
        CloseHandle(m_hStopEvent);
        m_hStopEvent = nullptr;
    }

    // テクスチャの解放はテクスチャマネージャーに任せる
    m_IconMap.clear();
	m_DefaultFileIcon = nullptr;
	m_DefaultFolderIcon = nullptr;
}

void ItemProjectWindow::DrawImGui()
{
    if(m_Kind != Kind::__ProjectWindow)
        return;

    if(m_NeedsRefresh)
    {
        m_NeedsRefresh = false;
        RefreshCurrentFolder();

#ifdef _DEBUG
        DebugManager::GetInstance().DebugLog("Project window refreshed");
#endif
    }

    // ツールバー
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));

    // 検索ボックス
    ImGui::SetNextItemWidth(300.0f);
    if(ImGui::InputTextWithHint("##ProjectSearch", "Search...", m_SearchBuffer, sizeof(m_SearchBuffer)))
    {
        RefreshCurrentFolder();
    }
    ImGui::SameLine();

    // フィルターボタン
    if(ImGui::Checkbox("All", &m_ShowAll))
    {
        m_ShowImages = m_ShowAll;
        m_ShowModels = m_ShowAll;
        m_ShowScripts = m_ShowAll;
        RefreshCurrentFolder();
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("Images", &m_ShowImages)) RefreshCurrentFolder();
    ImGui::SameLine();
    if(ImGui::Checkbox("Models", &m_ShowModels)) RefreshCurrentFolder();
    ImGui::SameLine();
    if(ImGui::Checkbox("Scripts", &m_ShowScripts)) RefreshCurrentFolder();
    ImGui::SameLine();

    // アイコンサイズスライダー
    ImGui::SetNextItemWidth(150.0f);
    ImGui::SliderFloat("##IconSize", &m_IconSize, 50.0f, 150.0f, "Size: %.0f");

    ImGui::PopStyleVar();
    ImGui::Separator();

    // メインエリア（左：フォルダツリー、右：ファイル一覧）
    ImGui::BeginChild("##FolderTree", ImVec2(250, 0), true);
    {
        DrawFolderTree(m_RootPath, true);
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("##FileGrid", ImVec2(0, 0), true);
    {
        // パンくずリスト
        DrawBreadcrumb();
        ImGui::Separator();

        // グリッド表示
        DrawFileGrid();
    }
    ImGui::EndChild();

    // 削除確認ダイアログ
    if(m_ShowDeleteConfirmation)
    {
        ImGui::OpenPopup("Delete Confirmation");

        if(ImGui::BeginPopupModal("Delete Confirmation", &m_ShowDeleteConfirmation, ImGuiWindowFlags_AlwaysAutoResize))
        {
            std::string targetName = m_DeleteTarget.filename().string();
            ImGui::Text("Are you sure you want to delete?");
            ImGui::Separator();
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "%s", targetName.c_str());
            ImGui::Separator();

            bool isFolder = std::filesystem::is_directory(m_DeleteTarget);
            if(isFolder)
            {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "This folder and all its contents will be deleted!");
            }

            ImGui::Spacing();

            if(ImGui::Button("Delete", ImVec2(120, 0)))
            {
                DeleteItem(m_DeleteTarget);

                // 削除したフォルダが選択中だった場合、親フォルダに移動
                if(m_CurrentPath == m_DeleteTarget || IsSubPath(m_CurrentPath, m_DeleteTarget))
                {
                    StopWatching();
                    m_CurrentPath = m_DeleteTarget.parent_path();
                    RefreshCurrentFolder();
                    StartWatching();
                }

                m_ShowDeleteConfirmation = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if(ImGui::Button("Cancel", ImVec2(120, 0)) || ImGui::IsKeyPressed(ImGuiKey_Escape))
            {
                m_ShowDeleteConfirmation = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }
}

void ItemProjectWindow::DrawFolderTree(_In_ const std::filesystem::path &In_Path, _In_ bool In_IsRoot)
{
    if(!std::filesystem::exists(In_Path) || !std::filesystem::is_directory(In_Path))
        return;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    // 選択中のフォルダならハイライト
    if(m_CurrentPath == In_Path)
        flags |= ImGuiTreeNodeFlags_Selected;

    // 子フォルダがない場合は Leaf
    bool hasSubFolders = false;
    try
    {
        for(const auto &entry : std::filesystem::directory_iterator(In_Path))
        {
            if(entry.is_directory())
            {
                hasSubFolders = true;
                break;
            }
        }
    }
    catch(...) {}

    if(!hasSubFolders)
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

    std::string label = In_IsRoot ? "Assets" : Util::ShiftJISToUTF8(In_Path.filename().string());

    // TreeNodeEx をラベルなしで描画
    bool nodeOpen = ImGui::TreeNodeEx(Util::ShiftJISToUTF8(In_Path.string()).c_str(), flags, "");

    // 同じ行にアイコンを配置
    ImGui::SameLine();

    float iconSize = ImGui::GetTextLineHeight(); // テキストの高さに合わせる

    if(m_DefaultFolderIcon)
    {
        // 画像アイコンを表示
        ImGui::Image(static_cast<void*>(m_DefaultFolderIcon->GetResource()), ImVec2(iconSize, iconSize));
    }

    // 同じ行にラベルを配置
    ImGui::SameLine();
    ImGui::Text("%s", label.c_str());

    // クリックでフォルダを開く
    if(ImGui::IsItemClicked())
    {
        m_CurrentPath = In_Path;
        RefreshCurrentFolder();
    }

	// 右クリックメニュー
    if(ImGui::BeginPopupContextItem("##TreeFolderContext"))
    {
        // 現在のフォルダ名を表示
        ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "%s", label.c_str());
        ImGui::Separator();

        // Open
        if(ImGui::MenuItem("Open"))
        {
            StopWatching();
            m_CurrentPath = In_Path;
            RefreshCurrentFolder();
            StartWatching();
        }

        // Open with Explorer
        if(ImGui::MenuItem("Open with Explorer"))
        {
            OpenInSystemExplorer(In_Path);
        }

        ImGui::Separator();

        // New Folder
        if(ImGui::MenuItem("New Folder"))
        {
            CreateNewFolderAt(In_Path);
        }

        ImGui::Separator();

        // Copy Path
        if(ImGui::MenuItem("Copy Path"))
        {
            std::filesystem::path absolutePath = std::filesystem::absolute(In_Path);
            ImGui::SetClipboardText(absolutePath.string().c_str());
        }

        // Copy Relative Path
        if(ImGui::MenuItem("Copy Relative Path"))
        {
            std::string relativePath = std::filesystem::relative(In_Path, m_RootPath).string();
            ImGui::SetClipboardText(relativePath.c_str());
        }

        // ルートフォルダ以外のみ編集可能
        if(!In_IsRoot)
        {
            ImGui::Separator();

            // Rename
            if(ImGui::MenuItem("Rename", "F2"))
            {
                m_IsRenaming = true;
                m_RenamingItem = In_Path;
                strncpy_s(m_RenameBuffer, In_Path.filename().string().c_str(), sizeof(m_RenameBuffer) - 1);
            }

            // Delete
            if(ImGui::MenuItem("Delete", "Del"))
            {
                // 削除確認ダイアログを表示
                m_ShowDeleteConfirmation = true;
                m_DeleteTarget = In_Path;
            }
        }

        ImGui::Separator();

        // Refresh
        if(ImGui::MenuItem("Refresh", "F5"))
        {
            RefreshCurrentFolder();
        }

        ImGui::EndPopup();
    }

    // 子フォルダを描画
    if(nodeOpen && hasSubFolders)
    {
        try
        {
            std::vector<std::filesystem::path> subFolders;
            for(const auto &entry : std::filesystem::directory_iterator(In_Path))
            {
                if(entry.is_directory())
                    subFolders.push_back(entry.path());
            }

            // ソート
            std::sort(subFolders.begin(), subFolders.end(),
                [](const auto &a, const auto &b)
                {
                    return a.filename().string() < b.filename().string();
                });

            for(const auto &folder : subFolders)
            {
                DrawFolderTree(folder, false);
            }
        }
        catch(...) {}

        ImGui::TreePop();
    }
}

void ItemProjectWindow::DrawBreadcrumb()
{
    // パンくずリスト
    std::vector<std::filesystem::path> pathParts;
    std::filesystem::path temp = m_CurrentPath;

    while(temp != m_RootPath && temp.has_parent_path())
    {
        pathParts.push_back(temp);
        temp = temp.parent_path();
    }
    pathParts.push_back(m_RootPath);

    std::reverse(pathParts.begin(), pathParts.end());

    for(size_t i = 0; i < pathParts.size(); ++i)
    {
        std::string label = (i == 0) ? "Assets" : pathParts[i].filename().string();

        if(ImGui::Button(label.c_str()))
        {
            m_CurrentPath = pathParts[i];
            RefreshCurrentFolder();
        }

        if(i < pathParts.size() - 1)
        {
            ImGui::SameLine();
            ImGui::Text(">");
            ImGui::SameLine();
        }
    }
}

void ItemProjectWindow::DrawFileGrid()
{
    ImGui::BeginChild("##GridContent");

    float windowWidth = ImGui::GetContentRegionAvail().x;
    float cellSize = m_IconSize + 10.0f;
    int columnsCount = std::max(1, static_cast<int>(windowWidth / cellSize));

    // すべてのアイテム（フォルダ + ファイル）
    std::vector<std::filesystem::path> allItems;
    allItems.insert(allItems.end(), m_CurrentFolders.begin(), m_CurrentFolders.end());
    allItems.insert(allItems.end(), m_CurrentFiles.begin(), m_CurrentFiles.end());

    int column = 0;
    bool IsAnyItemHovered = false;

    for(const auto &item : allItems)
    {
        ImGui::PushID(item.string().c_str());

        // グリッドの1セル
        ImGui::BeginGroup();

        bool IsFolder = std::filesystem::is_directory(item);
        bool isSelected = (m_SelectedItem == item);

        // 選択状態の背景色
        if(isSelected)
        {
            ImVec2 min = ImGui::GetCursorScreenPos();
            ImVec2 max = ImVec2(min.x + m_IconSize, min.y + m_IconSize + 25);
            ImDrawList *drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(min, max, IM_COL32(60, 120, 200, 100));
        }

        // アイコン表示
        auto iconTexture = GetFileIconTexture(item);

        if(iconTexture)
        {
            ImGui::Image(static_cast<void*>(iconTexture->GetResource()), ImVec2(m_IconSize, m_IconSize));
        }
        else
        {
            // テクスチャがない場合はダミー
            ImGui::Dummy(ImVec2(m_IconSize, m_IconSize));
            ImDrawList *drawList = ImGui::GetWindowDrawList();
            ImVec2 min = ImGui::GetItemRectMin();
            ImVec2 max = ImGui::GetItemRectMax();
            drawList->AddRect(min, max, IM_COL32(100, 100, 100, 255));

            // 中央にテキストアイコン
            std::string icon =IsFolder ? "Folder" : "File";
            ImVec2 textSize = ImGui::CalcTextSize(icon.c_str());
            ImVec2 textPos = ImVec2(
                min.x + (max.x - min.x - textSize.x) * 0.5f,
                min.y + (max.y - min.y - textSize.y) * 0.5f
            );
            drawList->AddText(textPos, IM_COL32(200, 200, 200, 255), icon.c_str());
        }

        // クリック判定
        if(ImGui::IsItemClicked())
        {
            m_SelectedItem = item;

            // ダブルクリック
            if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                if(IsFolder)
                {
                    m_CurrentPath = item;
                    RefreshCurrentFolder();
                }
                else if(m_FileSelectedCallback)
                {
                    m_FileSelectedCallback(item.string());
                }
            }
        }

        // ファイル名（アイコンの下）
        std::string filename = Util::ShiftJISToUTF8(item.filename().string());

        // 長いファイル名は省略
        ImVec2 textSize = ImGui::CalcTextSize(filename.c_str());
        if(textSize.x > m_IconSize)
        {
            // 省略表示
            std::string shortened = filename;
            while(ImGui::CalcTextSize((shortened + "...").c_str()).x > m_IconSize && shortened.length() > 3)
            {
                shortened.pop_back();
            }
            filename = shortened + "...";
        }

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (m_IconSize - ImGui::CalcTextSize(filename.c_str()).x) * 0.5f);
        ImGui::TextWrapped("%s", filename.c_str());

        ImGui::EndGroup();

        if(ImGui::IsItemHovered())
			IsAnyItemHovered = true;

        // 右クリックメニュー
        if(ImGui::BeginPopupContextItem("##ItemContext"))
        {
            if(IsFolder)
            {
                if(ImGui::MenuItem("Open"))
                {
                    m_CurrentPath = item;
                    RefreshCurrentFolder();
                }

                // エクスプローラーで開く
                if(ImGui::MenuItem("Open with Explorer"))
                {
					Util::OpenInSystemExplorer(item.string());
                }

                ImGui::Separator();
            }
            // エクスプローラーで表示
            if(ImGui::MenuItem("Show in Explorer"))
            {
                Util::ShowInSystemExplorer(item.string());
            }

            if(ImGui::MenuItem("Copy Path"))
            {
                // 絶対パスに変換
                std::filesystem::path absolutePath = std::filesystem::absolute(item);
                // Windows: パス区切り文字を \ に統一
                std::string pathStr = absolutePath.string();
                std::replace(pathStr.begin(), pathStr.end(), '/', '\\');

                ImGui::SetClipboardText(pathStr.c_str());
            }
            if(ImGui::MenuItem("Copy Relative Path"))
            {
                std::string relativePath = std::filesystem::relative(item, m_RootPath).string();
                ImGui::SetClipboardText(relativePath.c_str());
            }
            if(ImGui::MenuItem("Rename","F2"))
            {
                m_IsRenaming = true;
                m_RenamingItem = item;
                strncpy_s(m_RenameBuffer, item.filename().string().c_str(), sizeof(m_RenameBuffer) - 1);
            }
            if(ImGui::MenuItem("Delete","Del"))
            {
                // 削除確認ダイアログを表示
                m_ShowDeleteConfirmation = true;
                m_DeleteTarget = item;
            }
            ImGui::EndPopup();
        }

        ImGui::PopID();

        // グリッドレイアウト
        column++;
        if(column < columnsCount)
        {
            ImGui::SameLine();
        }
        else
        {
            column = 0;
        }
    }

    // キーボードショートカット
    if(ImGui::IsWindowFocused())
    {
        // F2: リネーム
        if(!m_SelectedItem.empty() && ImGui::IsKeyPressed(ImGuiKey_F2))
        {
            m_IsRenaming = true;
            m_RenamingItem = m_SelectedItem;
            strncpy_s(m_RenameBuffer, m_SelectedItem.filename().string().c_str(), sizeof(m_RenameBuffer) - 1);
        }

        // Delete: 削除
        if(!m_SelectedItem.empty() && ImGui::IsKeyPressed(ImGuiKey_Delete))
        {
            // 削除確認ダイアログを表示
            m_ShowDeleteConfirmation = true;
            m_DeleteTarget = m_SelectedItem;
        }
    }

    // ----- 空白領域の右クリックメニュー -----

    // 空白領域をクリックしたか判定
    bool IsWindowHovered = ImGui::IsWindowHovered();

    // 空白領域で左クリック → 選択解除
    if(IsWindowHovered && !IsAnyItemHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        m_SelectedItem.clear();
    }

    // 空白領域で右クリック → コンテキストメニューを開く
    if(IsWindowHovered && !IsAnyItemHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        ImGui::OpenPopup("##EmptyAreaContext");
    }

    // 空白領域のコンテキストメニュー
    if(ImGui::BeginPopup("##EmptyAreaContext"))
    {
        // 現在のフォルダパスを表示
        std::string currentFolder = m_CurrentPath.filename().string();
        if(currentFolder.empty())
            currentFolder = "Root";
        ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Current: %s", currentFolder.c_str());
        ImGui::Separator();

        if(ImGui::MenuItem("New Folder", "Ctrl+Shift+N"))
        {
            CreateNewFolder();
        }

        ImGui::Separator();

        if(ImGui::MenuItem("Refresh", "F5"))
        {
            RefreshCurrentFolder();
        }

        if(ImGui::MenuItem("Open in Explorer"))
        {
            OpenInSystemExplorer(m_CurrentPath);
        }

        ImGui::EndPopup();
    }

    ImGui::EndChild();

    // リネーム処理
    if(m_IsRenaming)
    {
        ImGui::OpenPopup("Rename");
        if(ImGui::BeginPopupModal("Rename", &m_IsRenaming, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("New name:");
            ImGui::SetNextItemWidth(300);

            if(ImGui::IsWindowAppearing())
                ImGui::SetKeyboardFocusHere();

            bool confirmed = ImGui::InputText("##RenameInput", m_RenameBuffer, sizeof(m_RenameBuffer),
                ImGuiInputTextFlags_EnterReturnsTrue);

            if(ImGui::Button("OK", ImVec2(120, 0)) || confirmed)
            {
                RenameItem(m_RenamingItem, m_RenameBuffer);
                m_IsRenaming = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if(ImGui::Button("Cancel", ImVec2(120, 0)) || ImGui::IsKeyPressed(ImGuiKey_Escape))
            {
                m_IsRenaming = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }
}

std::shared_ptr<Texture> ItemProjectWindow::GetFileIconTexture(_In_ const std::filesystem::path &In_Path)
{
    if(std::filesystem::is_directory(In_Path))
    {
        return m_DefaultFolderIcon;
    }

    std::string ext = In_Path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    auto it = m_IconMap.find(ext);
    if(it != m_IconMap.end())
    {
        return it->second;
    }

    return m_DefaultFileIcon;
}

void ItemProjectWindow::RefreshCurrentFolder()
{
    m_CurrentFiles.clear();
    m_CurrentFolders.clear();

    if(!std::filesystem::exists(m_CurrentPath) || !std::filesystem::is_directory(m_CurrentPath))
        return;

    try
    {
        for(const auto &entry : std::filesystem::directory_iterator(m_CurrentPath))
        {
            if(entry.is_directory())
            {
                m_CurrentFolders.push_back(entry.path());
            }
            else if(entry.is_regular_file())
            {
                if(PassFilter(entry.path()))
                    m_CurrentFiles.push_back(entry.path());
            }
        }

        std::sort(m_CurrentFolders.begin(), m_CurrentFolders.end(),
            [](const auto &a, const auto &b)
            {
                return a.filename().string() < b.filename().string();
            });

        std::sort(m_CurrentFiles.begin(), m_CurrentFiles.end(),
            [](const auto &a, const auto &b)
            {
                return a.filename().string() < b.filename().string();
            });
    }
    catch(const std::exception &e)
    {
        DebugManager::GetInstance().DebugLogError("Failed to refresh folder: {}", e.what());
    }
}

bool ItemProjectWindow::PassFilter(_In_ const std::filesystem::path &In_Path) const
{
    std::string filename = In_Path.filename().string();
    std::string ext = In_Path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    // 検索フィルター
    if(m_SearchBuffer[0] != '\0')
    {
        std::string search = m_SearchBuffer;
        std::transform(search.begin(), search.end(), search.begin(), ::tolower);
        std::string lowerFilename = filename;
        std::transform(lowerFilename.begin(), lowerFilename.end(), lowerFilename.begin(), ::tolower);

        if(lowerFilename.find(search) == std::string::npos)
            return false;
    }

    // 拡張子フィルター
    if(!m_ShowAll)
    {
        bool passImageFilter = m_ShowImages && (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".tga" || ext == ".dds");
        bool passModelFilter = m_ShowModels && (ext == ".fbx" || ext == ".obj" || ext == ".gltf" || ext == ".glb");
        bool passScriptFilter = m_ShowScripts && (ext == ".cpp" || ext == ".h" || ext == ".hpp" || ext == ".c" || ext == ".cs" || ext == ".py" || ext == ".lua");

        if(!passImageFilter && !passModelFilter && !passScriptFilter)
            return false;
    }

    return true;
}

void ItemProjectWindow::CreateNewFolder()
{
    std::string newFolderName = "NewFolder";
    std::filesystem::path newFolderPath = m_CurrentPath / newFolderName;

    int count = 1;
    while(std::filesystem::exists(newFolderPath))
    {
        newFolderPath = m_CurrentPath / (newFolderName + std::to_string(count));
        ++count;
    }

    try
    {
        std::filesystem::create_directory(newFolderPath);
        RefreshCurrentFolder();

        m_SelectedItem = newFolderPath;
        m_IsRenaming = true;
        m_RenamingItem = newFolderPath;
        strncpy_s(m_RenameBuffer, newFolderPath.filename().string().c_str(), sizeof(m_RenameBuffer) - 1);
    }
    catch(const std::exception &e)
    {
        DebugManager::GetInstance().DebugLogError("Failed to create folder: {}", e.what());
    }
}

void ItemProjectWindow::CreateNewFolderAt(_In_ const std::filesystem::path &In_ParentPath)
{
    std::string newFolderName = "NewFolder";
    std::filesystem::path newFolderPath = In_ParentPath / newFolderName;

    int count = 1;
    while(std::filesystem::exists(newFolderPath))
    {
        newFolderPath = In_ParentPath / (newFolderName + std::to_string(count));
        count++;
    }

    try
    {
        std::filesystem::create_directory(newFolderPath);

        DebugManager::GetInstance().DebugLog("Created new folder: {}", newFolderPath.string());

        // 現在のフォルダをリフレッシュ（作成したフォルダが表示されるように）
        RefreshCurrentFolder();

        // 作成したフォルダを選択してリネーム状態にする
        m_SelectedItem = newFolderPath;
        m_IsRenaming = true;
        m_RenamingItem = newFolderPath;
        strncpy_s(m_RenameBuffer, newFolderPath.filename().string().c_str(), sizeof(m_RenameBuffer) - 1);
    }
    catch(const std::exception &e)
    {
        DebugManager::GetInstance().DebugLogError("Failed to create folder: {}", e.what());
    }
}

void ItemProjectWindow::DeleteItem(_In_ const std::filesystem::path &In_Path)
{
    try
    {
        if(std::filesystem::is_directory(In_Path))
            std::filesystem::remove_all(In_Path);
        else
            std::filesystem::remove(In_Path);

        RefreshCurrentFolder();

        if(m_SelectedItem == In_Path)
            m_SelectedItem.clear();
    }
    catch(const std::exception &e)
    {
        DebugManager::GetInstance().DebugLogError("Failed to delete item: {}", e.what());
    }
}

void ItemProjectWindow::RenameItem(_In_ const std::filesystem::path &In_OldPath, _In_ const std::string &In_NewName)
{
    if(In_NewName.empty())
    {
        DebugManager::GetInstance().DebugLogWarning("Name cannot be empty");
        return;
    }

    std::filesystem::path newPath = In_OldPath.parent_path() / In_NewName;

    if(std::filesystem::exists(newPath))
    {
        DebugManager::GetInstance().DebugLogWarning("Item with this name already exists");
        return;
    }

    try
    {
        std::filesystem::rename(In_OldPath, newPath);
        RefreshCurrentFolder();

        if(m_SelectedItem == In_OldPath)
            m_SelectedItem = newPath;
    }
    catch(const std::exception &e)
    {
        DebugManager::GetInstance().DebugLogError("Failed to rename item: {}", e.what());
    }
}

bool ItemProjectWindow::IsSubPath(_In_ const std::filesystem::path &In_Path, _In_ const std::filesystem::path &In_BasePath)
{
    std::filesystem::path relativePath;

    try
    {
        relativePath = std::filesystem::relative(In_Path, In_BasePath);
    }
    catch(...)
    {
        return false;
    }

    // 相対パスが ".." で始まる場合はサブパスではない
    auto it = relativePath.begin();
    if(it != relativePath.end() && *it == "..")
    {
        return false;
    }

    return true;
}

void ItemProjectWindow::WatchFileSystemChanges()
{
    const DWORD bufferSize = 4096;
    BYTE buffer[bufferSize];

    while(m_IsWatching)
    {
        std::filesystem::path watchPath = m_CurrentPath;

        // パスが変わったらすぐに新しい監視を開始
        if(m_hDirectory != INVALID_HANDLE_VALUE && watchPath != m_CurrentPath)
        {
            DebugManager::GetInstance().DebugLog("Path changed during wait, canceling current watch");
            CancelIo(m_hDirectory);
            CloseHandle(m_hDirectory);
            m_hDirectory = INVALID_HANDLE_VALUE;
            continue;  // 新しいパスで監視を開始
        }

		HANDLE hDirectory = INVALID_HANDLE_VALUE;

		// ディレクトリハンドルを開く
        {
            std::lock_guard<std::mutex> lock(m_DirectoryHandleMutex);
            // ディレクトリハンドルを開く(非同期モード)
            hDirectory = CreateFileW(
                watchPath.wstring().c_str(),
                FILE_LIST_DIRECTORY,
                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                NULL,
                OPEN_EXISTING,
                FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,  // 非同期フラグ
                NULL
            );

            if(hDirectory == INVALID_HANDLE_VALUE)
            {
                DebugManager::GetInstance().DebugLogError("Failed to open directory for watching: {}", watchPath.string());
                std::this_thread::sleep_for(std::chrono::seconds(5));
                continue;
            }

            m_hDirectory = hDirectory;
        }

        // OVERLAPPEDをリセット
        ResetEvent(m_Overlapped.hEvent);

        DWORD bytesReturned = 0;

        // 非同期で変更を監視
        BOOL result = ReadDirectoryChangesW(
            hDirectory,
            buffer,
            bufferSize,
            TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME |
            FILE_NOTIFY_CHANGE_DIR_NAME |
            FILE_NOTIFY_CHANGE_SIZE |
            FILE_NOTIFY_CHANGE_LAST_WRITE,
            &bytesReturned,
            &m_Overlapped,  // OVERLAPPEDを渡す
            NULL
        );

        if(!result && GetLastError() != ERROR_IO_PENDING)
        {
            std::lock_guard<std::mutex> lock(m_DirectoryHandleMutex);
            DebugManager::GetInstance().DebugLogError("ReadDirectoryChangesW failed: {}", GetLastError());
            CloseHandle(hDirectory);
            m_hDirectory = INVALID_HANDLE_VALUE;
            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }

        // イベントを待つ(停止イベントまたは変更イベント)
        HANDLE waitHandles[2] = { m_hStopEvent, m_Overlapped.hEvent };
        DWORD waitResult = WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);

        if(waitResult == WAIT_OBJECT_0)
        {
            // 停止イベントがシグナルされた->終了
            CancelIoEx(hDirectory, &m_Overlapped);
            // I/Oのキャンセルを待つ
            DWORD bytesTransferred = 0;
            GetOverlappedResult(hDirectory, &m_Overlapped, &bytesTransferred, TRUE);

            std::lock_guard<std::mutex> lock(m_DirectoryHandleMutex);
            CloseHandle(hDirectory);
            m_hDirectory = INVALID_HANDLE_VALUE;
            break;
        }
        else if(waitResult == WAIT_OBJECT_0 + 1)
        {
            // 変更イベントがシグナルされた->変更を処理
            DWORD bytesTransferred = 0;
            if(GetOverlappedResult(hDirectory, &m_Overlapped, &bytesTransferred, FALSE))
            {
                if(bytesTransferred > 0)
                {
                    FILE_NOTIFY_INFORMATION *info = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(buffer);

                    do
                    {
                        std::wstring filename(info->FileName, info->FileNameLength / sizeof(WCHAR));

                        m_NeedsRefresh = true;

                        if(info->NextEntryOffset == 0)
                            break;
                        info = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(
                            reinterpret_cast<BYTE *>(info) + info->NextEntryOffset
                            );

                    } while(true);
                }
            }
        }

        // ハンドルを閉じる
        {
            std::lock_guard<std::mutex> lock(m_DirectoryHandleMutex);
            CloseHandle(m_hDirectory);
            m_hDirectory = INVALID_HANDLE_VALUE;
        }

        // パスが変更されたかチェック
        if(watchPath != m_CurrentPath)
            continue;

        // 監視継続フラグをチェック
        if(!m_IsWatching)
            break;
    }
}

void ItemProjectWindow::StartWatching()
{
    if(m_IsWatching)
        return;

    m_IsWatching = true;
    m_WatcherThread = std::thread(&ItemProjectWindow::WatchFileSystemChanges, this);
}

void ItemProjectWindow::StopWatching()
{
    if(!m_IsWatching)
        return;

    m_IsWatching = false;

	// 監視スレッドに停止を通知
    if(m_hStopEvent != nullptr)
    {
        SetEvent(m_hStopEvent);
    }

    // ハンドルをキャンセル
    {
        std::lock_guard<std::mutex> lock(m_DirectoryHandleMutex);
        if(m_hDirectory != INVALID_HANDLE_VALUE)
        {
            CancelIoEx(m_hDirectory, &m_Overlapped);
        }
    }

    // スレッドの終了を待つ
    if(m_WatcherThread.joinable())
    {
        m_WatcherThread.join();
    }
}

void ItemProjectWindow::SetRootPath(_In_ std::string_view In_RootPath)
{
    m_RootPath = In_RootPath;
    m_CurrentPath = m_RootPath;
    RefreshCurrentFolder();
}

std::string ItemProjectWindow::GetSelectedFilePath() const
{
    if(m_SelectedItem.empty())
        return "";
    return m_SelectedItem.string();
}

void ItemProjectWindow::SetFileSelectedCallback(_In_ std::function<void(const std::string &)> In_Callback)
{
    m_FileSelectedCallback = In_Callback;
}

void ItemProjectWindow::RegisterIcon(_In_ std::string_view In_Extension, _In_ std::shared_ptr<Texture> In_Texture)
{
    std::string ext = In_Extension.data();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    m_IconMap[ext] = In_Texture;
}

void ItemProjectWindow::SetDefaultFolderIcon(_In_ std::shared_ptr<Texture> In_Texture)
{
    m_DefaultFolderIcon = In_Texture;
}

void ItemProjectWindow::SetDefaultFileIcon(_In_ std::shared_ptr<Texture> In_Texture)
{
    m_DefaultFileIcon = In_Texture;
}