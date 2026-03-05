/*+===================================================================
	File: Engine.hpp
	Summary: エンジンの基本機能をまとめたヘッダーファイル
	Author: AT13C192 01 青木雄一郎
	Date: 2026/1/10 Sat AM 04:20:24 初回作成
===================================================================+*/
#pragma once

// ==============================
// Engine Info
// ==============================
namespace RyuuEngine
{
	constexpr const char *ENGINE_NAME = "RyuuEngine";
	constexpr const char *ENGINE_VERSION = "1.0.0";
	constexpr const char *AUTHOR = "ryuu3160";
}

// ==============================
// Core Defines & Utility
// ==============================
#include "Core/Defines.hpp"                              // エンジン全体の定義
#include "Core/System/Utility/Concept.hpp"               // C++20 コンセプト
#include "Core/System/Utility/Utility.hpp"               // ユーティリティ関数
#include "Core/System/Utility/EngineUtility.hpp"         // エンジン用ユーティリティ関数

// ==============================
// Math
// ==============================
#include "Core/DirectX11/System/DX11_Math.hpp"           // DirectX Math ヘルパー

// ==============================
// ryuu_lib(ユーティリティライブラリ)
// ==============================
#include "ryuu_lib/Singleton/Singleton.hpp"
#include "ryuu_lib/Singleton/SingletonController.hpp"
#include "ryuu_lib/RandomModule/Random.hpp"
#include "ryuu_lib/Easing/Easing.hpp"
#include "ryuu_lib/FrameManager/FrameManager.hpp"
#include "ryuu_lib/FrameManager/Timer/Timer.hpp"
#include "ryuu_lib/Observer/Observer.hpp"
#include "ryuu_lib/CRC31_HashGenerator/HashGenerator.hpp"
#include "ryuu_lib/MultiComparison/MultiComparison.hpp"
#include "ryuu_lib/WindowGenerator/Window.hpp"
#include "ryuu_lib/IME_Controller/IME.hpp"

// ==============================
// Core System
// ==============================

// DirectX Core
#include "Core/DirectX11/DX11_Core.hpp"                  // DirectX11 初期化

// Object(基底クラス)
#include "Core/System/Object/Object.hpp"                 // Object 基底
#include "Core/System/Object/GameObject.hpp"             // GameObject

// Component(基底クラス)
#include "Core/System/Component/Component.hpp"           // Component 基底
#include "Core/System/Component/Transform.hpp"           // Transform

// ==============================
// Components
// ==============================

// Camera
#include "Core/System/Component/Camera/Camera.hpp"

// Movement
#include "Core/System/Component/MovementComponent.hpp"
#include "Core/System/Component/PlayerController.hpp"

// Collider
#include "Core/System/Component/Collider/ColliderBase.hpp"
#include "Core/System/Component/Collider/BoxCollider.hpp"
#include "Core/System/Component/Collider/SphereCollider.hpp"
#include "Core/System/Component/Collider/CapsuleCollider.hpp"

// ==============================
// Managers
// ==============================
#include "Core/System/Managers/CameraManager.hpp"
#include "Core/System/Managers/SceneManager.hpp"
#include "Core/System/Managers/SoundManager.hpp"
#include "Core/System/Managers/FadeManager.hpp"

// CollisionManager
#include "Core/System/Managers/CollisionManager/TreeData.hpp"
#include "Core/System/Managers/CollisionManager/OctreeCell.hpp"
#include "Core/System/Managers/CollisionManager/CollisionManager.hpp"

// ==============================
// Rendering System
// ==============================

// Resource
#include "Core/DirectX11/Resource/ResourceSetting.hpp"
#include "Core/DirectX11/Resource/Texture.hpp"
#include "Core/DirectX11/Resource/Material.hpp"

// Mesh Resource
#include "Core/DirectX11/Resource/MesheResource/MeshBuffer.hpp"
#include "Core/DirectX11/Resource/MesheResource/Mesh.hpp"
#include "Core/DirectX11/Resource/MesheResource/InstancedMeshBuffer.hpp"
#include "Core/DirectX11/Resource/MesheResource/InstancedMesh.hpp"

// Shader Resource
#include "Core/DirectX11/Resource/ShaderResource/ShaderParam.hpp"
#include "Core/DirectX11/Resource/ShaderResource/Shader.hpp"
#include "Core/DirectX11/Resource/ShaderResource/UnorderedAccessView.hpp"

// Resource Manager
#include "Core/DirectX11/ResourceManager/TextureManager.hpp"
#include "Core/DirectX11/ResourceManager/MaterialManager.hpp"
#include "Core/DirectX11/ResourceManager/ShaderManager.hpp"
#include "Core/DirectX11/ResourceManager/ModelManager.hpp"
#include "Core/DirectX11/ResourceManager/InstancedModelManager.hpp"
#include "Core/DirectX11/ResourceManager/RenderTargetManager.hpp"

// Renderer
#include "Core/DirectX11/Renderer/RenderComponent.hpp"
#include "Core/DirectX11/Renderer/SpriteRenderer.hpp"
#include "Core/DirectX11/Renderer/ModelRenderer.hpp"
#include "Core/DirectX11/Renderer/InstancedModelRenderer.hpp"
#include "Core/DirectX11/Renderer/SkyBoxRenderer.hpp"
#include "Core/DirectX11/Renderer/LineRenderer.hpp"
#include "Core/DirectX11/Renderer/RTSpriteRenderer.hpp"

// Render System
#include "Core/DirectX11/System/RenderContext.hpp"
#include "Core/DirectX11/System/RenderManager.hpp"
#include "Core/DirectX11/System/Gizmos.hpp"

// ==============================
// Particle System
// ==============================
#include "Core/DirectX11/System/Particle/Core/Particle.hpp"
#include "Core/DirectX11/System/Particle/Core/ParticleEmitter.hpp"
#include "Core/DirectX11/System/Particle/Core/ParticlePoolManager.hpp"
#include "Core/DirectX11/System/Particle/Components/ParticleEffect.hpp"

// ==============================
// Input
// ==============================
#include "Core/System/Input/Input.hpp"
#include "Core/System/Input/InputSystem.hpp"

// ==============================
// Scene
// ==============================
#include "Core/System/Scene/SceneBase.hpp"
#include "Core/System/Scene/SceneRoot.hpp"

// ==============================
// SpriteManager(エディター機能)
// ==============================
#include "Core/System/SpriteManager/SpriteManager.hpp"
#include "Core/System/SpriteManager/SpriteManagerLoader.hpp"
#include "Core/System/SpriteManager/SpriteManagerHierarchy.hpp"
#include "Core/System/SpriteManager/SpriteManagerInspector.hpp"
#include "Core/System/SpriteManager/SpriteManagerSceneSelecter.hpp"
#include "Core/System/SpriteManager/SpriteManagerWindow.hpp"

// ==============================
// Engine Objects(プリセット)
// ==============================
#include "Core/System/Object/CameraObj.hpp"
#include "Core/System/Object/SkyBoxObj.hpp"
#include "Core/System/Object/GridObject.hpp"

// ==============================
// Entry Point
// ==============================
#include "EntryPoint/Main.hpp"
#include "EntryPoint/EntryPoint.hpp"