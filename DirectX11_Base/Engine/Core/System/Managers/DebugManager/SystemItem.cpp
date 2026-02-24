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
    if(ImGui::IsKeyPressed(ImGuiKey_F))
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
{
    m_Name = In_Name.data();
    m_Kind = Kind::__ProjectWindow; // プロジェクトウィンドウ用の特殊な Kind
    m_SearchBuffer[0] = '\0';
    m_RenameBuffer[0] = '\0';

    // ルートパスが存在しない場合は作成
    if(!std::filesystem::exists(m_RootPath))
    {
        std::filesystem::create_directories(m_RootPath);
    }

    RefreshCurrentFolder();
}

ItemProjectWindow::~ItemProjectWindow()
{
}

void ItemProjectWindow::DrawImGui()
{
    if(m_Kind != Kind::__Hierarchy)
        return;

    // ツールバー
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));

    // 検索ボックス
    ImGui::SetNextItemWidth(200.0f);
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

    ImGui::PopStyleVar();
    ImGui::Separator();

    // メインエリア（左：フォルダツリー、右：ファイル一覧）
    ImGui::BeginChild("##FolderTree", ImVec2(250, 0), true);
    {
        DrawFolderTree(m_RootPath, true);
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("##FileList", ImVec2(0, 0), true);
    {
        // 現在のパス表示
        std::string relativePath = std::filesystem::relative(m_CurrentPath, m_RootPath).string();
        if(relativePath.empty() || relativePath == ".")
            relativePath = "Root";
        ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Current: %s", relativePath.c_str());
        ImGui::Separator();

        DrawFileList();
    }
    ImGui::EndChild();

    // 右クリックメニュー
    DrawContextMenu();
}

void ItemProjectWindow::DrawFolderTree(const std::filesystem::path &In_Path, bool In_IsRoot)
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

    std::string label = In_IsRoot ? "Root" : In_Path.filename().string();
    bool nodeOpen = ImGui::TreeNodeEx(In_Path.string().c_str(), flags, "%s %s", ICON_FOLDER, label.c_str());

    // クリックでフォルダを開く
    if(ImGui::IsItemClicked())
    {
        m_CurrentPath = In_Path;
        RefreshCurrentFolder();
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

void ItemProjectWindow::DrawFileList()
{
    // フォルダ一覧
    for(const auto &folder : m_CurrentFolders)
    {
        ImGui::PushID(folder.string().c_str());

        bool isSelected = (m_SelectedItem == folder);

        if(ImGui::Selectable((std::string(ICON_FOLDER) + " " + folder.filename().string()).c_str(),
            isSelected, ImGuiSelectableFlags_AllowDoubleClick))
        {
            m_SelectedItem = folder;

            // ダブルクリックでフォルダを開く
            if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                m_CurrentPath = folder;
                RefreshCurrentFolder();
            }
        }

        // 右クリックメニュー
        if(ImGui::BeginPopupContextItem())
        {
            if(ImGui::MenuItem("Open"))
            {
                m_CurrentPath = folder;
                RefreshCurrentFolder();
            }
            if(ImGui::MenuItem("Rename"))
            {
                m_IsRenaming = true;
                m_RenamingItem = folder;
                strncpy_s(m_RenameBuffer, folder.filename().string().c_str(), sizeof(m_RenameBuffer) - 1);
            }
            if(ImGui::MenuItem("Delete"))
            {
                DeleteItem(folder);
            }
            ImGui::EndPopup();
        }

        ImGui::PopID();
    }

    // ファイル一覧
    for(const auto &file : m_CurrentFiles)
    {
        ImGui::PushID(file.string().c_str());

        bool isSelected = (m_SelectedItem == file);

        std::string icon = GetFileIcon(file);
        std::string displayName = icon + " " + file.filename().string();

        if(ImGui::Selectable(displayName.c_str(), isSelected, ImGuiSelectableFlags_AllowDoubleClick))
        {
            m_SelectedItem = file;

            // ダブルクリックでファイルを開く（コールバック呼び出し）
            if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && m_FileSelectedCallback)
            {
                m_FileSelectedCallback(file.string());
            }
        }

        // 右クリックメニュー
        if(ImGui::BeginPopupContextItem())
        {
            if(ImGui::MenuItem("Copy Path"))
            {
                ImGui::SetClipboardText(file.string().c_str());
            }
            if(ImGui::MenuItem("Copy Relative Path"))
            {
                std::string relativePath = std::filesystem::relative(file, m_RootPath).string();
                ImGui::SetClipboardText(relativePath.c_str());
            }
            if(ImGui::MenuItem("Rename"))
            {
                m_IsRenaming = true;
                m_RenamingItem = file;
                strncpy_s(m_RenameBuffer, file.filename().string().c_str(), sizeof(m_RenameBuffer) - 1);
            }
            if(ImGui::MenuItem("Delete"))
            {
                DeleteItem(file);
            }
            ImGui::EndPopup();
        }

        ImGui::PopID();
    }

    // リネーム処理
    if(m_IsRenaming)
    {
        ImGui::OpenPopup("Rename");
        if(ImGui::BeginPopupModal("Rename", &m_IsRenaming, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("New name:");
            ImGui::SetNextItemWidth(300);
            bool confirmed = ImGui::InputText("##RenameInput", m_RenameBuffer, sizeof(m_RenameBuffer),
                ImGuiInputTextFlags_EnterReturnsTrue);

            if(ImGui::Button("OK", ImVec2(120, 0)) || confirmed)
            {
                RenameItem(m_RenamingItem, m_RenameBuffer);
                m_IsRenaming = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if(ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                m_IsRenaming = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }
}

void ItemProjectWindow::DrawContextMenu()
{
    // 空白領域の右クリックメニュー
    if(ImGui::BeginPopupContextWindow("##ProjectContextMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
    {
        if(ImGui::MenuItem("New Folder"))
        {
            CreateNewFolder();
        }

        if(ImGui::MenuItem("Refresh"))
        {
            RefreshCurrentFolder();
        }

        if(ImGui::MenuItem("Open in Explorer"))
        {
            std::string command = "explorer " + m_CurrentPath.string();
            system(command.c_str());
        }

        ImGui::EndPopup();
    }
}

const char *ItemProjectWindow::GetFileIcon(const std::filesystem::path &In_Path)
{
    std::string ext = In_Path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    // 画像ファイル
    if(ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".tga" || ext == ".dds")
        return ICON_IMAGE;

    // モデルファイル
    if(ext == ".fbx" || ext == ".obj" || ext == ".gltf" || ext == ".glb")
        return ICON_MODEL;

    // スクリプトファイル
    if(ext == ".cpp" || ext == ".h" || ext == ".hpp" || ext == ".c" || ext == ".cs" || ext == ".py" || ext == ".lua")
        return ICON_SCRIPT;

    // テキストファイル
    if(ext == ".txt" || ext == ".md" || ext == ".json" || ext == ".xml" || ext == ".csv")
        return ICON_TEXT;

    // シェーダーファイル
    if(ext == ".hlsl" || ext == ".glsl" || ext == ".shader")
        return ICON_SHADER;

    // オーディオファイル
    if(ext == ".wav" || ext == ".mp3" || ext == ".ogg" || ext == ".flac")
        return ICON_AUDIO;

    return ICON_FILE;
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

        // ソート
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

bool ItemProjectWindow::PassFilter(const std::filesystem::path &In_Path)
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

    // 同名のフォルダが存在する場合は番号を付ける
    int count = 1;
    while(std::filesystem::exists(newFolderPath))
    {
        newFolderPath = m_CurrentPath / (newFolderName + std::to_string(count));
        count++;
    }

    try
    {
        std::filesystem::create_directory(newFolderPath);
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

void ItemProjectWindow::DeleteItem(const std::filesystem::path &In_Path)
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

void ItemProjectWindow::RenameItem(const std::filesystem::path &In_OldPath, const std::string &In_NewName)
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