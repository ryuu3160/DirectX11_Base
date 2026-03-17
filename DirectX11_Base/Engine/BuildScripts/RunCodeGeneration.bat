@echo off
setlocal enabledelayedexpansion

REM =========================================
REM 引数を受け取る
REM =========================================
set "PROJECT_DIR=%~1"
set "OUTPUT_FILE=%~2"

REM =========================================
REM デフォルト値を設定
REM =========================================
if "%PROJECT_DIR%"=="" (
    REM スクリプトのディレクトリから親ディレクトリを取得
    set "PROJECT_DIR=%~dp0..\.."
)

if "%OUTPUT_FILE%"=="" (
    set "OUTPUT_FILE=Engine\Core\System\Generated\ComponentRegistry_Generated.cpp"
)

REM =========================================
REM パスを正規化（絶対パスに変換）
REM =========================================
pushd "%PROJECT_DIR%"
set "PROJECT_DIR=%CD%"
popd

REM OUTPUT_FILE が相対パスなら絶対パスに変換
if not "%OUTPUT_FILE:~1,1%"==":" (
    set "OUTPUT_FILE=%PROJECT_DIR%\%OUTPUT_FILE%"
)

REM =========================================
REM Python のパスを決定
REM =========================================
set "PYTHON_EMBEDDED=%PROJECT_DIR%\Engine\ThirdParty\Python\python.exe"
set "PYTHON_SCRIPT=%~dp0GenerateComponentRegistry.py"

echo.
echo ========================================
echo Run Code Generation.
echo ========================================

if exist "%PYTHON_EMBEDDED%" (
    set "PYTHON_EXE=%PYTHON_EMBEDDED%"
    echo Using embedded Python: %PYTHON_EXE%
) else (
    where python >nul 2>&1
    if !ERRORLEVEL! EQU 0 (
        set "PYTHON_EXE=python"
        echo Using system Python: %PYTHON_EXE%
    ) else (
        echo.
        echo ERROR: Python not found!
        echo Please install Python or place python.exe in Tools\Python\
        exit /b 1
    )
)

REM =========================================
REM Python バージョン確認
REM =========================================
echo.
echo ========================================
echo Check Python Version
echo ========================================
echo Python version:
"%PYTHON_EXE%" --version

REM =========================================
REM Python スクリプト実行
REM =========================================
echo.
echo Executing Python script...

set PYTHONIOENCODING=shift-jis
"%PYTHON_EXE%" "%PYTHON_SCRIPT%" --source-dir "%PROJECT_DIR%\Engine" --output "%OUTPUT_FILE%" --function-name RegisterAllEngineComponents --registry-mode singleton

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: Failed to generate component registry
    exit /b 1
)

echo.
echo ========================================
echo Generation Complete!
echo ========================================
exit /b 0