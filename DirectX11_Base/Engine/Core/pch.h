/*+===================================================================
	File: pch.h
	Summary: プリコンパイルヘッダー
	Author: AT12C192 01 青木雄一郎
	Date: 11/15/2024 Fri AM 01:38:39 初回作成
===================================================================+*/
#pragma once

#include <cassert>
#include <cstdio>
#include <Windows.h>

// メモリリークの検出
#include <crtdbg.h>

// COM
#include <initguid.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

// timer
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

// ------------------------------
//	STL
// ------------------------------
#include <fstream>
#include <filesystem>
#include <sstream>
#include <string>
#include <cstring>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <typeindex>
#include <memory>
#include <algorithm>
#include <functional>
#include <any>
#include <variant>
#include <mutex>
#include <cmath>
#include <random>
#include <future>
#include <optional>
#include <stdexcept>

// C++20以降の機能を利用するための条件付きインクルード
#if __cplusplus >= 202002L
#include <utility>
#include <type_traits>
#include <concepts>
#endif

// -------------------------------
//  STL コンテナ
// -------------------------------
#include <vector>
#include <array>
#include <map>
#include <list>
#include <unordered_map>
#include <deque>
#include <initializer_list>
#include <forward_list>
#include <set>
#include <unordered_set>
#include <iterator>

// ------------------------------
//  DirectX11
// ------------------------------
#include <DirectXMath.h>

// ------------------------------
//  OpenMP
// ------------------------------
#ifdef _OPENMP
#include <omp.h>
#endif

// ------------------------------
//  ryuu_lib
// ------------------------------
#include "ryuu_lib/Singleton/Singleton.hpp"
#include "ryuu_lib/WindowGenerator/Window.hpp"
#include "ryuu_lib/RandomModule/Random.hpp"
#include "ryuu_lib/Observer/Observer.hpp"
#include "ryuu_lib/IME_Controller/IME.hpp"
#include "ryuu_lib/FrameManager/FrameManager.hpp"
#include "ryuu_lib/Easing/Easing.hpp"
#include "ryuu_lib/CRC31_HashGenerator/HashGenerator.hpp"
#include "ryuu_lib/MultiComparison/MultiComparison.hpp"

// ------------------------------
//  Original
// ------------------------------
#include "Engine/Core/System/Utility/Concept.hpp"
#include "Engine/Core/System/CPON/cpon.hpp"
#include "EntryPoint/Main.hpp"
#include "Defines.hpp"
#include "Engine/Core/System/Utility/Utility.hpp"
#include "Engine/Core/DirectX11/DX11_Core.hpp"
#include "Engine/Core/System/Managers/CameraManager.hpp"
#include "Engine/Core/System/Managers/DebugManager/DebugManager.hpp"
#include "Engine/Core/DirectX11/ResourceManager/RenderTargetManager.hpp"
#include "Engine/Core/System/Object/GameObject.hpp"
#include "Engine/Core/System/Component/Component.hpp"
#include "Engine/Core/System/Input/Input.hpp"
#include "Engine/Core/DirectX11/System/DX11_Math.hpp"
#include "Engine/Core/System/Managers/FadeManager.hpp"
#include "Engine/Core/System/Managers/SceneManager.hpp"
#include "Engine/Core/System/Managers/SoundManager.hpp"
