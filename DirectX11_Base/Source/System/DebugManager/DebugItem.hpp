/*+===================================================================
	File: DebugItem.hpp
	Summary: デバッグ項目クラス
	Author: AT13C192 01 青木雄一郎
	Date: 2025/11/15 Sat PM 04:00:12 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================

// ==============================
//	定数定義
// ==============================
namespace
{

}

/// <summary>
/// DebugItemクラス
/// </summary>
class DebugItem
{
public:
	union Value {
		bool				flg;
		int					nValue;
		float				fValue;
		DirectX::XMFLOAT2	vector2;
		DirectX::XMFLOAT3	vector;
		DirectX::XMFLOAT4	color;
		std::string			string;
	};

	enum Kind {
		Label,		// 項目のみの表示
		Bool,		// チェックボックス
		Int,		// 整数入力
		Float,		// 小数入力
		Vector,		// ベクター入力
		Color,		// 色入力
		Path,		// ファイルパスの指定
		Command,	// ボタン
		Group,		// 表示項目をまとめる
		List,		// 一覧表示
	};

protected:
	DebugItem();

public:
	~DebugItem();

	std::string GetName() const;

	const char *GetCStrName() const;

	/// <summary>
	/// 項目の種別を取得
	/// </summary>
	/// <returns>種類</returns>
	Kind GetKind() const;
	// 各種設定値の取得
	bool GetBool() const;
	int GetInt() const;
	float GetFloat() const;
	DirectX::XMFLOAT3 GetVector() const;
	DirectX::XMFLOAT4 GetColor() const;
	std::string GetStr() const;

public:
	// 文字列から項目の種別を取得
	static Kind StrToKind(std::string str);
	// 項目の種別から文字列を取得
	static std::string KindToStr(Kind kind);


private:
	std::string m_Name;
	Kind m_Kind;
};

class ItemValue : public DebugItem
{
public:
	ItemValue();
	~ItemValue();

private:
	Value m_Value;
	bool m_IsSave;
};
