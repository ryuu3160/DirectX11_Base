/*+===================================================================
	File: Concept.hpp
	Summary: コンセプトの定義をまとめたヘッダ
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/23 Sun AM 01:51:55 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================

#if __cplusplus >= 202002L
template<typename T>
concept TypePOD = std::is_trivial_v<T> && std::is_standard_layout_v<T>;

template<typename T>
concept TypeString = std::is_same_v<T, std::string> || std::is_same_v<T, const char *>;


#endif
