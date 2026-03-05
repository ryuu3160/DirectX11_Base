/*+===================================================================
	File: pch.h
	Summary: プリコンパイルヘッダー
	Author: AT12C192 01 青木雄一郎
	Date: 11/15/2024 Fri AM 01:38:39 初回作成
===================================================================+*/
#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
// ==============================
// C/C++ 標準ライブラリ
// ==============================
#include <cassert>
#include <cstdio>
#include <cmath>
#include <cstring>

// ==============================
// Windows API
// ==============================
#include <Windows.h>
#undef max  // Windows.hのmaxマクロを無効化
#undef min  // Windows.hのminマクロを無効化

// メモリリーク検出
#include <crtdbg.h>

// COM
#include <initguid.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

// タイマー
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

// ==============================
// DirectX11
// ==============================
#include <DirectXMath.h>
#include <d3d11.h>

// ==============================
// STL
// ==============================

// I/O
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

// 文字列
#include <string>

// コンテナ
#include <vector>
#include <array>
#include <list>
#include <deque>
#include <forward_list>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>

// イテレータ
#include <iterator>
#include <initializer_list>

// スマートポインタ・関数オブジェクト
#include <memory>
#include <functional>
#include <algorithm>

// その他
#include <typeindex>
#include <any>
#include <variant>
#include <optional>
#include <chrono>
#include <filesystem>
#include <random>
#include <mutex>
#include <future>
#include <stdexcept>

// C++20 以降
#if __cplusplus >= 202002L
#include <utility>
#include <type_traits>
#include <concepts>
#endif

// ==============================
// OpenMP(オプション)
// ==============================
#ifdef _OPENMP
#include <omp.h>
#endif

// ==============================
// サードパーティライブラリ
// ==============================
// ImGuiは変更頻度が低いのでpchに含めても良い
#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_impl_dx11.h"
#include "ThirdParty/ImGui/imgui_impl_win32.h"

// NlohmannJsonも変更頻度が低い
#include "ThirdParty/NlohmannJson/nlohmann/json.hpp"

// CPONも変更頻度が低い
#include "ThirdParty/CPON/cpon.hpp"
