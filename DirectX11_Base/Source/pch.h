/*+===================================================================
	File: pch.h
	Summary: プリコンパイルヘッダー
	Author: AT12C192 01 青木雄一郎
	Date: 11/15/2024 Fri AM 01:38:39 初回作成
===================================================================+*/
#pragma once

#include <Windows.h>
#include <stdio.h>
#include <assert.h>

// メモリリークの検出
#include <crtdbg.h>

// COM
#include <wrl/client.h>
#include <initguid.h>
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
#include <memory>
#include <algorithm>
#include <functional>
#include <any>
#include <mutex>
#include <cmath>
#include <random>
#include <future>
#include <sal.h>

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
#include "DirectX11/DX11_Initialize.hpp"
#include "System/Object/GameObject.hpp"
#include "System/Component/Component.hpp"
#include "System/Input/Input.hpp"
#include "DirectX11/System/DX11_Math.hpp"
#include "System/Scene/SceneManager.hpp"
#include "System/Sound/SoundManager.hpp"
