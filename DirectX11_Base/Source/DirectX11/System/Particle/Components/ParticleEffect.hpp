/*+===================================================================
	File: ParticleEffect.hpp
	Summary: （このファイルで何をするか記載する）
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/28 Sun AM 11:21:59 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "DirectX11/Renderer/RenderComponent.hpp"
#include "DirectX11/System/Particle/Core/ParticleEmitter.hpp"
#include "DirectX11/Resource/Meshes/InstancedMeshBuffer.hpp"

/// <summary>
/// ParticleEffectクラス
/// </summary>
class ParticleEffect : public RenderComponent
{
public:
    ParticleEffect();
    ~ParticleEffect();

    void Init() noexcept override;
    void Update(_In_ float In_Tick) noexcept override;
    void Draw(_In_ RenderContext *In_RenderContext) noexcept override;

    /// <summary>
    /// エミッターを追加
    /// </summary>
    ParticleEmitter *AddEmitter(_In_ const EmitterSettings &In_Settings);

    /// <summary>
    /// すべてのエミッターを再生
    /// </summary>
    void Play();

    /// <summary>
    /// すべてのエミッターを停止
    /// </summary>
    void Stop();

    /// <summary>
    /// 重力設定
    /// </summary>
    void SetGravity(_In_ const DirectX::XMFLOAT3 &In_Gravity) { m_Gravity = In_Gravity; }
    const DirectX::XMFLOAT3 &GetGravity() const noexcept { return m_Gravity; }

    /// <summary>
    /// テクスチャ設定
    /// </summary>
    void SetTexture(_In_ const FilePath &In_Path);

private:
    /// <summary>
    /// インスタンスバッファの更新
    /// </summary>
    void UpdateInstanceBuffer();

    /// <summary>
    /// デフォルトシェーダーの作成
    /// </summary>
    void MakeDefaultShader();

private:
    std::vector<std::unique_ptr<ParticleEmitter>> m_Emitters;
    std::shared_ptr<InstancedMeshBuffer> m_InstanceBuffer;
    std::shared_ptr<Texture> m_Texture;

    DirectX::XMFLOAT3 m_Gravity;

    InstancedVertexShader *m_pVS;
    PixelShader *m_pPS;

    struct InstanceData
    {
        DirectX::XMFLOAT4X4 World;
        DirectX::XMFLOAT4 Color;
    };
    std::vector<InstanceData> m_InstanceDataArray;
};
