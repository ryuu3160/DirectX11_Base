/*+===================================================================
	File: SoundManager.hpp
	Summary: サウンドマネージャークラス
	Author:  井上楓太、青木雄一郎
	Date: 2025/9/9 Tue AM 01:15:11 初回作成
===================================================================+*/
#pragma once

// ==============================
//	include
// ==============================
#include <xaudio2.h>
#include <mmreg.h>
#include <MSAcm.h>
#include <Shlwapi.h>

#pragma comment(lib, "msacm32.lib")
#pragma comment(lib, "shlwapi.lib")

/// <summary>
/// SoundManagerクラス
/// </summary>
class SoundManager : public Singleton<SoundManager>
{
	friend class Singleton<SoundManager>;
public:
#undef PlaySound // winapiのPlaySoundを無効にする

	/// <summary>
	/// サウンドファイルの読み込み
	/// </summary>
	/// <param name="[In_strFileName]">サウンドネーム</param>
	/// <param name="[In_c_cpFilePath]">ファイル</param>
	/// <param name="[In_bIsSE]">SEかどうか true = SE</param>
	/// <param name="[In_bLoop]">ループ</param>
	/// <returns>サウンドバッファ</returns>
	XAUDIO2_BUFFER *Load(_In_ const std::string &In_strSoundName, _In_ const FilePath &In_strFileName,
		_In_ const bool &In_bIsSE = true, _In_ const bool &In_bLoop = false) noexcept;

	/// <summary>
	/// ロードしたサウンドを再生する
	/// </summary>
	/// <param name="[In_strFileName]">サウンド名</param>
	void Play(_In_ const std::string &In_strSoundName) noexcept;

	/// <summary>
	/// サウンドが再生中かどうか
	/// </summary>
	/// <param name="[In_strFileName]">サウンド名</param>
	bool IsPlaying(_In_ const std::string &In_strSoundName) const noexcept;

	/// <summary>
	/// サウンドの停止
	/// </summary>
	/// <param name="[In_strFileName]">サウンド名</param>
	/// <param name="[In_bIsFlag]">trueなら引数以外のサウンドを停止する</param>
	void Stop(_In_ const std::string &In_strSoundName, _In_ bool In_bIsFlag = false) noexcept;

	/// <summary>
	/// ロードしたサウンドの音量を設定する
	/// </summary>
	/// <param name="[In_strFileName]">サウンド名</param>
	/// <param name="[In_nVolume]">音量</param>
	void SetVolume(_In_ const std::string &In_strSoundName, _In_ float In_nVolume) noexcept;

	/// <summary>
	/// サウンドの解放
	/// </summary>
	/// <param name="[In_strFile]">サウンド名</param>
	void UnLoad(_In_ const std::string &In_strSoundName);

	// ------------------------------
	//	getter
	// ------------------------------
	/// <summary>
	/// XAudio2のインターフェースを取得する
	/// <returns>XAudio2のインターフェース</returns>
	/// </summary>
	inline IXAudio2 *GetXAudio() const noexcept { return m_pXAudio.Get(); }

	/// <summary>
	/// 現在のBGMの音量を取得する
	/// </summary>
	/// <returns>音量</returns>
	inline const float &GetBGMVolume() const noexcept { return m_fBGMVolume; }

	/// <summary>
	/// 現在のSEの音量を取得する
	/// </summary>
	/// <returns>音量</returns>
	inline const float &GetSEVolume() const noexcept { return m_fSEVolume; }

	// ------------------------------
	//	setter
	// ------------------------------
	/// <summary>
	/// SEの音量を設定する
	/// </summary>
	/// <param name="[In_nVolume]">音量</param>
	void SetSEVolume(_In_ const float &In_nVolume) noexcept;

	/// <summary>
	/// BGMの音量を設定する
	///	</summary>
	/// <param name="[In_nVolume]">音量</param>
	void SetBGMVolume(_In_ const float &In_nVolume) noexcept;

private:
	/// <summary>
	/// 音源情報構造体
	/// </summary>
	struct Data
	{
		std::string strFilePath;			//ファイルパス
		WAVEFORMATEX	format{};			// WAVフォーマット
		std::shared_ptr<BYTE> pBuffer{};	// サウンドデータ
		DWORD			bufSize{};			// データサイズ
		XAUDIO2_BUFFER	sound{};			// サウンドバッファ
	};

	/// <summary>
	/// サウンド構造体
	/// </summary>
	struct SoundData
	{
		Data data{};
		IXAudio2SourceVoice *pSourceVoice{};
		float fVolume{};
		bool bIsSE{};
	};

	/// <summary>
	/// MP3フォーマット情報
	/// </summary>
	struct MP3FormatInfo
	{
		DWORD offset;
		DWORD dataSize;
	};

	/// <summary>
	/// MP3フレーム情報
	/// </summary>
	struct MP3FrameInfo
	{
		BYTE channel;
		BYTE padding;
		DWORD sampleRate;
		DWORD bitRate;
		DWORD frameSize;
	};

private:
	SoundManager() noexcept;
	~SoundManager();

	// ------------------------------
	//			メンバ変数
	// ------------------------------
	const BYTE CMP_MATCH;	// 定数
	using SoundKey = std::pair<std::string, SoundData>;	// サウンドキー
	using SoundMap = std::unordered_map<std::string, SoundData>;	// サウンドマップ
	ComPtr<IXAudio2> m_pXAudio;
	IXAudio2MasteringVoice *m_pMasterVoice;
	SoundMap m_mapSound;
	float m_fBGMVolume;
	float m_fSEVolume;

	/// <summary>
	/// wavファイルの読み込み
	/// </summary>
	/// <param name="[In_c_cpFile]">読み込むファイル</param>
	/// <param name="[In_pData]">サウンドデータ</param>
	/// <returns>成功していたらS_OK</returns>
	HRESULT LoadWav(_In_ const char *In_c_cpFile, _In_ SoundData *In_pData) noexcept;

	/// <summary>
	/// mp3ファイルの読み込み
	/// </summary>
	/// <param name="[In_c_cpFile]">読み込むファイル</param>
	/// <param name="[In_pData]">サウンドデータ</param>
	/// <returns>成功していたらS_OK</returns>
	HRESULT LoadMP3(_In_ const char *In_c_cpFile, _In_ SoundData *In_pData) noexcept;

	/// <summary>
	/// MP3フォーマットチェック
	/// </summary>
	/// <param name="[In_hFile]">ファイルポインタ</param>
	/// <param name="[In_pFormat]">ファイルフォーマット</param>
	/// <returns>データサイズ</returns>
	DWORD ReadMP3Format(_In_ HANDLE In_hFile, _In_ MP3FormatInfo *In_pFormat) const noexcept;

	/// <summary>
	/// MP3サウンドフレームヘッダ読み込み
	/// </summary>
	/// <param name="[hFile]">ファイルポインタ</param>
	/// <param name="[In_dwSeek]">フレーム読み込み位置</param>
	/// <param name="[In_pFrame]">フレーム情報</param>
	/// <returns>読み込みサイズ</returns>
	DWORD ReadMP3FrameHeader(_In_ HANDLE In_hFile, _In_ DWORD In_dwSeek, _In_ MP3FrameInfo *Out_pFrame) noexcept;

	/// <summary>
	/// MP3サウンドデータ読み込み
	/// </summary>
	/// <param name="[In_hFile]">ファイルポインタ</param>
	/// <param name="[In_dwSeek]">フレーム読み込み位置</param>
	/// <param name="[In_dwSize]">読み込みデータサイズ</param>
	/// <param name="[In_pFrame]">フレーム情報</param>
	/// <param name="[In_pData]">サウンドデータ</param>
	/// <returns>読み込みサイズ</returns>
	DWORD ReadMP3Data(_In_ HANDLE In_hFile, _In_ DWORD In_dwSeek, _In_ DWORD In_dwSize, _In_ MP3FrameInfo *In_pFrame, _In_ SoundData *In_pData) noexcept;
};
