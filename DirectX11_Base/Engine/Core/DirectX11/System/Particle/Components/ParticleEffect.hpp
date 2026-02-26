/*+===================================================================
	File: ParticleEffect.hpp
	Summary: パーティクルエフェクトコンポーネント
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/28 Sun AM 11:21:59 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include "Core/DirectX11/Renderer/RenderComponent.hpp"
#include "Core/DirectX11/System/Particle/Core/ParticleEmitter.hpp"
#include "Core/DirectX11/Resource/MesheResource/InstancedMeshBuffer.hpp"
#include "Core/DirectX11/Resource/MesheResource/InstancedMesh.hpp"

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
    /// デフォルトのパーティクルエミッターを追加します
    /// </summary>
    /// <returns>追加されたパーティクルエミッターへのポインター</returns>
    ParticleEmitter *AddEmitter();

	void RemoveEmitter(_In_ ParticleEmitter *In_pEmitter) noexcept;

	void RemoveEmitter(_In_ int In_Index) noexcept;

	/// <summary>
	/// すべてのエミッターをクリアします
	/// </summary>
	void ClearEmitters() noexcept { m_Emitters.clear(); }

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

protected:
#ifdef _DEBUG
    virtual void Inspector(_In_ ItemGroup *In_pGroup) override;
#endif

private:
    /// <summary>
    /// インスタンスバッファの更新
    /// </summary>
    void UpdateInstanceBuffer();

    /// <summary>
    /// デフォルトシェーダーの作成
    /// </summary>
    void MakeDefaultShader();

    /// <summary>
    /// 板ポリゴンメッシュの作成
    /// </summary>
    void CreateQuadMesh();

private:
    /// <summary>
    /// 板ポリゴン用の頂点構造体
    /// </summary>
    struct ParticleVertex
    {
        DirectX::XMFLOAT3 Position;
        DirectX::XMFLOAT2 TexCoord;
    };

    /// <summary>
    /// インスタンスデータ構造体
    /// </summary>
    struct InstanceData
    {
        DirectX::XMFLOAT4X4 World;
        DirectX::XMFLOAT4 Color;
    };

    std::shared_ptr<InstancedMeshBuffer> m_InstanceBuffer;
    std::shared_ptr<Texture> m_Texture;
    std::vector<std::unique_ptr<ParticleEmitter>> m_Emitters;
    std::vector<InstanceData> m_InstanceDataArray;
	std::vector<int> m_RemoveIndices; // 削除するインスタンスのインデックスリスト

    DirectX::XMFLOAT3 m_Gravity;
    bool m_bMeshInitialized; // メッシュが初期化済みか

    InstancedVertexShader *m_pVS;
    PixelShader *m_pPS;
};
