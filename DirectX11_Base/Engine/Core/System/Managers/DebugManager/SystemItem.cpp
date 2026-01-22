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
#include "Core/System/Object/GameObject.hpp"
#include "Core/System/Scene/SceneBase.hpp"
#include "Core/System/Managers/DebugManager/DebugManager.hpp"
// ==============================
//	定数定義
// ==============================
namespace
{
}

ItemHierarchy::ItemHierarchy(_In_ std::string In_Name, _In_ SceneBase *In_pScene, _In_ SelectCallback In_Func)
    : m_pScene(In_pScene)
    , m_SelectedObject(nullptr)
    , m_DraggedObject(nullptr)
    , m_SelectCallback(In_Func)
	, m_RenamingObject(nullptr)
	, m_RenameJustStarted(false)
{
    m_Name = In_Name;
    m_Kind = Kind::__Hierarchy;
    m_RenameBuffer[0] = '\0';
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