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
#include <iostream>
#include <iomanip>
#include <typeindex>
#include <memory>
#include <algorithm>
#include <functional>
#include <any>
#include <mutex>
#include <cmath>
#include <random>
#include <future>

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
//  ryuu_lib
// ------------------------------
#include "System/ryuu_lib/Singleton/Singleton.hpp"
#include "System/ryuu_lib/WindowGenerator/Window.hpp"
#include "System/ryuu_lib/RandomModule/Random.hpp"
#include "System/ryuu_lib/Observer/Observer.hpp"
#include "System/ryuu_lib/IME_Controller/IME.hpp"
#include "System/ryuu_lib/FrameManager/FrameManager.hpp"
#include "System/ryuu_lib/Easing/Easing.hpp"
#include "System/ryuu_lib/CRC31_HashGenerator/HashGenerator.hpp"

// ------------------------------
//  Original
// ------------------------------
#include "App/Main.hpp"
#include "Defines.hpp"
#include "DirectX11/DX11_Core.hpp"
#include "DirectX11/ResourceManager/RenderTargetManager.hpp"
#include "System/Object/GameObject.hpp"
#include "System/Component/Component.hpp"
#include "System/Input/Input.hpp"
#include "DirectX11/System/DX11_Math.hpp"
#include "System/FadeManager/FadeManager.hpp"
#include "System/Scene/SceneManager.hpp"
#include "System/Sound/SoundManager.hpp"
