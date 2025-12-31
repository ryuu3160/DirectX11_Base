@echo off
setlocal
REM 文字コードのモード取得
for /f "tokens=4 delims=: " %%a in ('chcp') do set "OLDCP=%%a"

REM バッチ自身のあるフォルダ取得
set "MYDIR=%~dp0"

REM .vcxprojファイルを探して変数に入れる
set "VCXPROJ="
for %%f in ("%MYDIR%*.vcxproj") do (
    set "VCXPROJ=%%~nxf"
    goto :found
)
:found

REM ユーザーに「Source/System」などを入力させる
set "USER_PATH="
chcp 65001 >nul
echo SpriteManagerフォルダを追加したフォルダへのパスを入力してください
echo ProjectDir/Source/System/SpriteManagerのようなフォルダ構成の場合は、Source/Systemと入力
set /p USER_PATH=Input: 

REM SpriteManager用のフルパスを自動生成
set "SPRITE_DIR=$(ProjectDir)/%USER_PATH%/SpriteManager"

REM 文字コードのモードを戻す
chcp %OLDCP% >nul

REM ここでPowerShellを実行
powershell -ExecutionPolicy Bypass -File add_include.ps1 "%MYDIR%%VCXPROJ%" "%SPRITE_DIR%"

REM スクリプトが終わったら不要なファイルを削除
del "%MYDIR%add_include.ps1"

REM バッチ自身を削除（start使って自分の実行完了後に消す）
start /b "" cmd /c del "%~f0"

endlocal
pause
