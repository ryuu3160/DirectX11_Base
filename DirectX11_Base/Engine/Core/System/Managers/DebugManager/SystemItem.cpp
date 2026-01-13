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
{
    m_Name = In_Name;
    m_Kind = Kind::__Hierarchy;
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
}

void ItemHierarchy::DrawRootObjects()
{
    if(!m_pScene)
        return;

    // シーン内のすべてのオブジェクトを取得
    const auto &allObjects = m_pScene->GetHierarchyObjects();

	// オブジェクトをルートから描画
    for(auto obj : allObjects)
    {
        if(obj && obj->GetTransform()->GetParent() == nullptr)
        {
            DrawObjectNode(obj);
        }
    }
}

void ItemHierarchy::DrawObjectNode(_Inout_ GameObject *obj)
{
    if(!obj)
        return;

    // ツリーノードのフラグ
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_OpenOnDoubleClick |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    // 選択状態
    if(m_SelectedObject == obj)
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    // 子がいない場合は葉ノード
    Transform *transform = obj->GetTransform();
    if(transform->GetChildCount() == 0)
    {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    // アイコン
    std::string label = obj->GetName();

    // ドラッグソースとして設定
    HandleDragSource(obj);

    // ツリーノードを描画
    bool nodeOpen = ImGui::TreeNodeEx(obj, flags, "%s", label.c_str());

    // ドロップターゲットとして設定
    HandleDropTarget(obj);

    // クリックで選択
    if(ImGui::IsItemClicked())
    {
        SelectObject(obj);
    }

    // 右クリックメニュー
    if(ImGui::BeginPopupContextItem())
    {
        ShowContextMenu(obj);
        ImGui::EndPopup();
    }

    // 子ノードを描画
    if(nodeOpen && transform->GetChildCount() > 0)
    {
        const auto &children = transform->GetChildren();
        for(Transform *childTransform : children)
        {
            GameObject *childObj = childTransform->GetGameObject();
            DrawObjectNode(childObj);
        }

        ImGui::TreePop();
    }
}

void ItemHierarchy::HandleDragSource(_In_ GameObject *obj)
{
    // ドラッグ開始
    if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
    {
        // ペイロードを設定（GameObject のポインタ）
        ImGui::SetDragDropPayload("HIERARCHY_OBJECT", &obj, sizeof(GameObject *));

        // ドラッグ中の表示
        ImGui::Text("Move:  %s", obj->GetName().c_str());

        m_DraggedObject = obj;

        ImGui::EndDragDropSource();
    }
}

void ItemHierarchy::HandleDropTarget(_In_ GameObject *obj)
{
    // ドロップターゲット
    if(ImGui::BeginDragDropTarget())
    {
        if(const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("HIERARCHY_OBJECT"))
        {
            GameObject *DroppedObj = *static_cast<GameObject **>(payload->Data);

            if(DroppedObj && DroppedObj != obj)
            {
                // 親子関係を設定
                Transform *DroppedTransform = DroppedObj->GetTransform();
                Transform *TargetTransform = obj->GetTransform();

                // 循環参照チェック(objがdroppedObj の子孫でないか)
                bool isCircular = false;
                Transform *ancestor = TargetTransform;
                while(ancestor != nullptr)
                {
                    if(ancestor == DroppedTransform)
                    {
                        isCircular = true;
                        break;
                    }
                    ancestor = ancestor->GetParent();
                }

                if(!isCircular)
                {
                    // objを親に設定(ワールド座標を維持)
                    DroppedTransform->SetParent(TargetTransform, true);
                }
            }
        }

        ImGui::EndDragDropTarget();
    }
}

void ItemHierarchy::ShowContextMenu(_In_ GameObject *obj)
{
    if(ImGui::MenuItem("Delete"))
    {
        // オブジェクトを削除
        obj->DestroySelf();
        if(m_SelectedObject == obj)
        {
            m_SelectedObject = nullptr;
            if(m_SelectCallback)
                m_SelectCallback(nullptr);
        }
    }

    ImGui::Separator();

    if(obj->GetTransform()->GetParent() != nullptr)
    {
        if(ImGui::MenuItem("Detach from Parent"))
        {
            // 親から外す
            obj->GetTransform()->SetParent(nullptr);
        }

        ImGui::Separator();
    }

    if(ImGui::MenuItem("Rename"))
    {

    }
}

void ItemHierarchy::SelectObject(_Inout_ GameObject *obj)
{
    m_SelectedObject = obj;
    if(m_SelectCallback)
    {
        m_SelectCallback(obj);
    }
}