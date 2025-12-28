/*+===================================================================
	File: ParticleEmitter.cpp
	Summary: ParticleEmitterクラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/12/28 Sun AM 11:21:32 初回作成
===================================================================+*/

// ==============================
//	include
// ==============================
#include "ParticleEmitter.hpp"

ParticleEmitter::ParticleEmitter()
    : m_EmitTimer(0.0f), m_DurationTimer(0.0f)
    , m_IsPlaying(false), m_RandomEngine(std::random_device{}())
{
    m_Particles.reserve(m_Settings.MaxParticles);
}

void ParticleEmitter::Init(_In_ const EmitterSettings &In_Settings)
{
}

void ParticleEmitter::Play()
{
	m_IsPlaying = true;
	m_DurationTimer = 0.0f;
}

void ParticleEmitter::Stop()
{
	m_IsPlaying = false;
}

void ParticleEmitter::Update(_In_ float In_DeltaTime, _In_ const DirectX::XMFLOAT3 &In_Gravity)
{
    // 既存パーティクルの更新
    for(auto &particle : m_Particles)
    {
        if(particle.m_IsActive)
            particle.Update(In_DeltaTime, In_Gravity);
    }

    // 非アクティブなパーティクルを削除
    m_Particles.erase(
        std::remove_if(m_Particles.begin(), m_Particles.end(),
            [](const Particle &p) { return !p.m_IsActive; }),
        m_Particles.end()
    );

    if(!m_IsPlaying) return;

    // 放出時間のチェック
    if(m_Settings.Duration > 0.0f)
    {
        m_DurationTimer += In_DeltaTime;
        if(m_DurationTimer >= m_Settings.Duration)
        {
            if(m_Settings.IsLooping)
            {
                m_DurationTimer = 0.0f;
            }
            else
            {
                m_IsPlaying = false;
                return;
            }
        }
    }

    // パーティクル放出
    m_EmitTimer += In_DeltaTime;
    float emitInterval = 1.0f / m_Settings.EmitRate;

    while(m_EmitTimer >= emitInterval && m_Particles.size() < m_Settings.MaxParticles)
    {
        EmitParticle();
        m_EmitTimer -= emitInterval;
    }
}

void ParticleEmitter::EmitParticle()
{
    Particle particle;
    particle.m_IsActive = true;
    particle.m_Position = GeneratePosition();
    particle.m_Velocity = GenerateVelocity();
    particle.m_Color = m_Settings.ColorStart;
    particle.m_Size = m_Settings.SizeStart;
    particle.m_MaxLifeTime = RandomRange(m_Settings.LifeTimeMin, m_Settings.LifeTimeMax);
    particle.m_GravityScale = m_Settings.GravityScale;

    m_Particles.push_back(particle);
}

DirectX::XMFLOAT3 ParticleEmitter::GeneratePosition()
{
    DirectX::XMFLOAT3 offset(0, 0, 0);

    switch(m_Settings.EmitShape)
    {
    case EmitterSettings::Shape::Sphere:
    {
        float r = RandomRange(0.0f, m_Settings.ShapeRadius);
        float theta = RandomRange(0.0f, DirectX::XM_2PI);
        float phi = RandomRange(0.0f, DirectX::XM_PI);

        offset.x = r * sinf(phi) * cosf(theta);
        offset.y = r * cosf(phi);
        offset.z = r * sinf(phi) * sinf(theta);
        break;
    }
    case EmitterSettings::Shape::Box:
    {
        offset.x = RandomRange(-m_Settings.ShapeSize.x * 0.5f, m_Settings.ShapeSize.x * 0.5f);
        offset.y = RandomRange(-m_Settings.ShapeSize.y * 0.5f, m_Settings.ShapeSize.y * 0.5f);
        offset.z = RandomRange(-m_Settings.ShapeSize.z * 0.5f, m_Settings.ShapeSize.z * 0.5f);
        break;
    }
    // Point の場合は offset (0,0,0) のまま
    }

    DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&m_Settings.Position);
    DirectX::XMVECTOR off = DirectX::XMLoadFloat3(&offset);
    DirectX::XMFLOAT3 result;
    DirectX::XMStoreFloat3(&result, DirectX::XMVectorAdd(pos, off));
    return result;
}

DirectX::XMFLOAT3 ParticleEmitter::GenerateVelocity()
{
    DirectX::XMFLOAT3 vel;
    vel.x = RandomRange(m_Settings.VelocityMin.x, m_Settings.VelocityMax.x);
    vel.y = RandomRange(m_Settings.VelocityMin.y, m_Settings.VelocityMax.y);
    vel.z = RandomRange(m_Settings.VelocityMin.z, m_Settings.VelocityMax.z);
    return vel;
}

float ParticleEmitter::RandomRange(_In_ float In_Min, _In_ float In_Max)
{
    std::uniform_real_distribution<float> dist(In_Min, In_Max);
    return dist(m_RandomEngine);
}
