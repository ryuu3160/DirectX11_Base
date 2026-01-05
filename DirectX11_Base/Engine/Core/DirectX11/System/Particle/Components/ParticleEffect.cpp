/*+===================================================================
	File: ParticleEffect.cpp
	Summary: パーティクルエフェクトコンポーネント
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/28 Sun AM 11:22:17 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "ParticleEffect.hpp"
#include "Engine/Core/DirectX11/ResourceManager/TextureManager.hpp"
#include "Engine/Core/DirectX11/ResourceManager/ShaderManager.hpp"

ParticleEffect::ParticleEffect()
    : RenderComponent("ParticleEffect")
    , m_Gravity(0.0f, -9.8f, 0.0f)
    , m_pVS(nullptr), m_pPS(nullptr)
	, m_bMeshInitialized(false)
{
    MakeDefaultShader();
}

ParticleEffect::~ParticleEffect()
{
    m_Emitters.clear();
}

void ParticleEffect::Init() noexcept
{
    RenderComponent::Init();

    if(!m_bMeshInitialized)
    {
        CreateQuadMesh();
        m_bMeshInitialized = true;
    }
}

void ParticleEffect::Update(_In_ float In_Tick) noexcept
{
    // 全エミッターの更新
    for(auto &emitter : m_Emitters)
    {
        if(emitter)
        {
            // エミッターの位置をGameObjectの位置に同期
            emitter->GetSettings().Position = m_pGameObject->GetPosition();
            emitter->Update(In_Tick, m_Gravity);
        }
    }

    // インスタンスバッファの更新
    UpdateInstanceBuffer();
}

void ParticleEffect::Draw(_In_ RenderContext *In_RenderContext) noexcept
{
    if(m_InstanceDataArray.empty() || !m_InstanceBuffer)
        return;

    auto &DX11 = DX11_Core::GetInstance();

	// パーティクル用のレンダーステート設定
	DX11.SetBlendMode(BlendMode::BLEND_ADD); // 加算合成
	DX11.SetDepthTest(DepthState::DEPTH_ENABLE_TEST); // 書き込み無効
	DX11.SetCullingMode(D3D11_CULL_NONE); // カリング無効

	// シェーダー設定
    DirectX::XMFLOAT4X4 mat[3];
    mat[0] = DirectX::XMFLOAT4X4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    ); // 単位行列
    mat[1] = In_RenderContext->GetView();
    mat[2] = In_RenderContext->GetProj();

    m_pVS->WriteBuffer(0, mat);
    m_pVS->SetInstanceSRV(m_InstanceBuffer->GetInstanceSRV());
    m_pVS->Bind();

    if(m_Texture)
        m_pPS->SetTexture(0, m_Texture.get());
    m_pPS->Bind();

    // 描画
    m_InstanceBuffer->Draw();

	// レンダーステートを元に戻す
    DX11.SetBlendMode(BlendMode::BLEND_ALPHA);
	DX11.SetDepthTest(DepthState::DEPTH_ENABLE_WRITE_TEST);
	DX11.SetCullingMode(D3D11_CULL_BACK);
}

ParticleEmitter *ParticleEffect::AddEmitter(_In_ const EmitterSettings &In_Settings)
{
    auto emitter = std::make_unique<ParticleEmitter>();
    emitter->Init(In_Settings);
    m_Emitters.push_back(std::move(emitter));
    return m_Emitters.back().get();
}

void ParticleEffect::Play()
{
    for(auto &emitter : m_Emitters)
    {
        if(emitter)
            emitter->Play();
    }
}

void ParticleEffect::Stop()
{
    for(auto &emitter : m_Emitters)
    {
        if(emitter)
            emitter->Stop();
    }
}

void ParticleEffect::SetTexture(_In_ const FilePath &In_Path)
{
    m_AssetPath = In_Path.data();
	m_Texture = TextureManager::GetInstance().LoadTexture(In_Path);
}

#ifdef _DEBUG
void ParticleEffect::RegisterDebugInspector(_In_ DebugWindow *In_pWindow)
{
    RenderComponent::RegisterDebugInspector(In_pWindow);
    auto &group = In_pWindow->GetGroupItem(m_Name);
    group.CreateGroupItem<ItemCallback>("Play", DebugItem::Command, [this](bool IsSet, void *ptr)
        {
            this->Play();
        });
    group.CreateGroupItem<ItemSameLine>();
    group.CreateGroupItem<ItemCallback>("Stop", DebugItem::Command, [this](bool IsSet, void *ptr)
        {
            this->Stop();
        });
    group.CreateGroupItem<ItemIndent>();
    
    // エミッター
    int num = 0;
    for(auto &itr : m_Emitters)
    {
        std::string Name = "Emitter";
        Name += ToString(num);
        auto EmitterGroup = group.CreateGroupItem<ItemGroup>(Name);
        auto &Settings = itr->GetSettings();
        EmitterGroup->CreateGroupItem<ItemBind>("Position", DebugItem::Vector, &Settings.Position);
        EmitterGroup->CreateGroupItem<ItemBind>("Rate", DebugItem::Float, &Settings.EmitRate);
        EmitterGroup->CreateGroupItem<ItemBind>("MaxParticle", DebugItem::Int, &Settings.MaxParticles);
        EmitterGroup->CreateGroupItem<ItemBind>("Duration", DebugItem::Float, &Settings.Duration);
        EmitterGroup->CreateGroupItem<ItemBind>("IsLoop", DebugItem::Bool, &Settings.IsLooping);
        EmitterGroup->CreateGroupItem<ItemBind>("LifeTimeMin", DebugItem::Float, &Settings.LifeTimeMin);
        EmitterGroup->CreateGroupItem<ItemBind>("LifeTimeMax", DebugItem::Float, &Settings.LifeTimeMax);
        EmitterGroup->CreateGroupItem<ItemBind>("GravityScale", DebugItem::Float, &Settings.GravityScale);
        EmitterGroup->CreateGroupItem<ItemBind>("Radius", DebugItem::Float, &Settings.ShapeRadius);
        EmitterGroup->CreateGroupItem<ItemBind>("Angle", DebugItem::Float, &Settings.ShapeAngle);
        ++num;
    }
    group.CreateGroupItem<ItemUnIndent>();
}
#endif

void ParticleEffect::UpdateInstanceBuffer()
{
    m_InstanceDataArray.clear();

    // 全エミッターから全パーティクルを収集
    for(auto &emitter : m_Emitters)
    {
        if(!emitter) continue;

        const auto &particles = emitter->GetParticles();
        for(const auto &particle : particles)
        {
            if(!particle->m_IsActive) continue;

            InstanceData data;

            // ワールド行列作成 (ビルボード化)
            DirectX::XMMATRIX S = DirectX::XMMatrixScaling(particle->m_Size.x, particle->m_Size.y, 1.0f);
            DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(
                particle->m_Position.x,
                particle->m_Position.y,
                particle->m_Position.z
            );

            // ビルボード (カメラ方向を向く)
            // 簡易版:  Y軸ビルボード
            DirectX::XMMATRIX World = S * T;
            DirectX::XMStoreFloat4x4(&data.World, DirectX::XMMatrixTranspose(World));

            data.Color = particle->m_Color;

            m_InstanceDataArray.push_back(data);
        }
    }

    if(m_InstanceDataArray.empty())
        return;

    DirectX::XMFLOAT3 cameraPos = CameraManager::GetInstance().GetMainCameraObj()->GetPosition();

    std::sort(m_InstanceDataArray.begin(), m_InstanceDataArray.end(),
        [&cameraPos](const InstanceData &a, const InstanceData &b)
        {
            // ワールド行列から位置を抽出
            DirectX::XMFLOAT3 posA(a.World._41, a.World._42, a.World._43);
            DirectX::XMFLOAT3 posB(b.World._41, b.World._42, b.World._43);

            // カメラからの距離を計算
            float distA = DirectX::XMVectorGetX(DirectX::XMVector3Length(
                DirectX::XMVectorSubtract(
                    DirectX::XMLoadFloat3(&posA),
                    DirectX::XMLoadFloat3(&cameraPos)
                )
            ));
            float distB = DirectX::XMVectorGetX(DirectX::XMVector3Length(
                DirectX::XMVectorSubtract(
                    DirectX::XMLoadFloat3(&posB),
                    DirectX::XMLoadFloat3(&cameraPos)
                )
            ));

            return distA > distB; // 遠い順
        });

    // インスタンスバッファの作成/更新
    if(m_InstanceBuffer)
    {
        m_InstanceBuffer->UpdateInstanceBuffer(
            m_InstanceDataArray.data(),
            sizeof(InstanceData),
            static_cast<UINT>(m_InstanceDataArray.size())
        );
    }
}

void ParticleEffect::MakeDefaultShader()
{
    // インスタンシング用シェーダーを使用
    m_defVS = std::make_shared<InstancedVertexShader>();
    m_defVS->Load("Binaries/Shader/IVS_Particle.cso");
    m_pVS = reinterpret_cast<InstancedVertexShader *>(m_defVS.get());

    m_defPS = std::make_shared<PixelShader>();
    m_defPS->Load("Binaries/Shader/PS_Particle.cso");
    m_pPS = m_defPS.get();
}

void ParticleEffect::CreateQuadMesh()
{
    // 頂点データの作成
    ParticleVertex vertices[] = {
        // Position              TexCoord
        { { -0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f } }, // 左上
        { {  0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f } }, // 右上
        { { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f } }, // 左下
        { {  0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f } }, // 右下
    };

	// インデックスデータの作成
    WORD indices[] = {
        0, 1, 2, // 1つ目の三角形
        2, 1, 3, // 2つ目の三角形
    };

    InstanceData dummyInstance = {};
    dummyInstance.World = DirectX::XMFLOAT4X4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
    dummyInstance.Color = DirectX::XMFLOAT4(1, 1, 1, 1);

	// Descriptionの作成
    InstancedMeshBuffer::InstancingDesc desc = {};

    // 頂点バッファ
    desc.pVtx = vertices;
    desc.vtxSize = sizeof(ParticleVertex);
    desc.vtxCount = 4;
    desc.isWrite = false;
    // インデックスバッファ
    desc.pIdx = indices;
    desc.idxSize = sizeof(WORD);
    desc.idxCount = 6;
    desc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    // インスタンスバッファ
    desc.pInstance = &dummyInstance;
    desc.instanceSize = sizeof(InstanceData);
    desc.instanceCount = 1;
    desc.isInstanceWrite = true;

	// InstancedMeshBufferの作成
    m_InstanceBuffer = std::make_shared<InstancedMeshBuffer>(desc);
}
