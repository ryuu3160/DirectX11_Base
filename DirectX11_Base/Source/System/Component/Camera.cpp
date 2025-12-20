/*+===================================================================
	File: Camera.cpp
	Summary: カメラのコンポーネント
	Author: AT13C192 01 青木雄一郎
	Date: 2025/07/03 Thu PM 02:11:41 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "Camera.hpp"
#include "DirectX11/System/Geometory.hpp"
#include "System/Object/GameObject.hpp"

Camera::Camera()
	: Component("Camera")
	, m_bIs3D(true), m_bIsLockZ(true)
	, m_fFovy(60.0f), m_fWidth(20.0f)
	, m_fAspect(16.0f / 9.0f), m_fNear(0.2f), m_fFar(1000.0f)
	, m_fFocus(1.0f)
#ifdef _DEBUG
	, m_bIsShow(false)
#endif
{
}

Camera::~Camera()
{
}

void Camera::Init() noexcept
{
}

void Camera::SaveLoad(_In_ DataAccessor *In_Data)
{
	In_Data->AccessValue<bool>("Is3D", & m_bIs3D);
	In_Data->AccessValue<float>("Fovy", &m_fFovy);
	In_Data->AccessValue<float>("Width", &m_fWidth);
	In_Data->AccessValue<float>("Near", &m_fNear);
	In_Data->AccessValue<float>("Far", &m_fFar);
	In_Data->AccessValue<float>("Aspect", &m_fAspect);
	In_Data->AccessValue<float>("Focus", &m_fFocus);
#ifdef _DEBUG
	In_Data->AccessValue<bool>("IsShow", & m_bIsShow);
#endif
}

#ifdef _DEBUG
void Camera::Draw() const noexcept
{
	if (!m_bIsShow) return;
	DirectX::XMFLOAT3 look = GetLook();
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(look.x, look.y, look.z);
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(0.1f, 0.1f, 0.1f);
	DirectX::XMFLOAT4X4 mat;
	DirectX::XMStoreFloat4x4(&mat, DirectX::XMMatrixTranspose(S * T));
	Gizmos::GetInstance().SetWorld(mat);
	Gizmos::GetInstance().DrawBox();
}
#endif

DirectX::XMFLOAT4X4 Camera::GetView(_In_ bool In_Transpose) const noexcept
{
	DirectX::XMFLOAT3 pos = m_pTransform->GetPosition();
	DirectX::XMFLOAT3 look = GetLook();
	DirectX::XMFLOAT3 up;
	if (m_bIsLockZ)
		up = { 0.0f, 1.0f, 0.0f };
	else
		up = m_pTransform->GetUp();
	DirectX::XMVECTOR vPos = DirectX::XMLoadFloat3(&pos);
	DirectX::XMVECTOR vLook = DirectX::XMLoadFloat3(&look);
	DirectX::XMVECTOR vUp = DirectX::XMLoadFloat3(&up);
	DirectX::XMMATRIX mat = DirectX::XMMatrixLookAtLH(vPos, vLook, vUp);

	// Transposeするかどうか
	if (In_Transpose)
		mat = DirectX::XMMatrixTranspose(mat);
	DirectX::XMFLOAT4X4 fmat;
	DirectX::XMStoreFloat4x4(&fmat, mat);
	return fmat;
}

DirectX::XMFLOAT4X4 Camera::GetProj(_In_ bool In_Transpose) const noexcept
{
	DirectX::XMMATRIX mat;
	if (m_bIs3D)
	{
		mat = DirectX::XMMatrixPerspectiveFovLH(GetFovy(), m_fAspect, m_fNear, m_fFar);
	}
	else
	{
		mat = DirectX::XMMatrixOrthographicLH(m_fWidth, m_fWidth / m_fAspect, m_fNear, m_fFar);
	}
	// Transposeするかどうか
	if (In_Transpose)
		mat = DirectX::XMMatrixTranspose(mat);
	DirectX::XMFLOAT4X4 fmat;
	DirectX::XMStoreFloat4x4(&fmat, mat);
	return fmat;
}

DirectX::XMFLOAT3 Camera::GetLook() const noexcept
{
	// 位置の取得
	DirectX::XMFLOAT3 pos = m_pTransform->GetPosition();
	DirectX::XMVECTOR vPos = DirectX::XMLoadFloat3(&pos);

	// 前方ベクトル取得
	DirectX::XMFLOAT3 front = m_pTransform->GetFront();
	DirectX::XMVECTOR vFront = DirectX::XMLoadFloat3(&front);

	// カメラの位置からフォーカス距離まで進んだ位置を注視点とする
	vFront = DirectX::XMVectorScale(vFront, m_fFocus);
	DirectX::XMVECTOR vLook = DirectX::XMVectorAdd(vPos, vFront);

	DirectX::XMStoreFloat3(&pos, vLook);
	return pos;
}

#ifdef _DEBUG
void Camera::RegisterDebugInspector(_In_ DebugWindow *In_pWindow)
{
	auto *group = In_pWindow->CreateItem<ItemGroup>("Camera Component");
	group->CreateGroupItem<ItemBind>("is3D", DebugItem::Kind::Bool, &m_bIs3D);
	group->CreateGroupItem<ItemBind>("Fovy", DebugItem::Kind::Float, &m_fFovy);
	group->CreateGroupItem<ItemBind>("Focus", DebugItem::Kind::Float, &m_fFocus);
	group->CreateGroupItem<ItemBind>("Width", DebugItem::Kind::Float, &m_fWidth);
	group->CreateGroupItem<ItemBind>("Near", DebugItem::Kind::Float, &m_fNear);
	group->CreateGroupItem<ItemBind>("Far", DebugItem::Kind::Float, &m_fFar);
	group->CreateGroupItem<ItemBind>("Aspect", DebugItem::Kind::Float, &m_fAspect);
	group->CreateGroupItem<ItemBind>("isShow", DebugItem::Kind::Bool, &m_bIsShow);
}
#endif
