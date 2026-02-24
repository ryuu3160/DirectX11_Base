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
        // カメラの forward ベクトルを取得
        DirectX::XMFLOAT3 Forward = m_pEditorCamera->GetTransform()->GetFront();

        // オブジェクトの位置
        DirectX::XMFLOAT3 TargetPos = m_SelectedObject->GetTransform()->GetPosition();

        // フォーカス距離
        float FocusDistance = 5.0f;

        // 新しいカメラ位置
        DirectX::XMFLOAT3 newCameraPos = TargetPos - Forward * FocusDistance;

        // カメラの位置を更新（回転は変えない）
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
