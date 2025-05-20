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
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <algorithm>
#include <array>
#include <functional>
#include <map>
#include <any>
#include <list>
#include <mutex>
#include <cmath>

// ------------------------------
//  Original
// ------------------------------

// ------------------------------
//  ryuu_lib
// ------------------------------
#include "System/ryuu_lib/Singleton/Singleton.hpp"
#include "System/ryuu_lib/WindowGenerator/Window.hpp"

