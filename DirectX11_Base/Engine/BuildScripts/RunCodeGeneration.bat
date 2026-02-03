@echo off
chcp 932 >nul
setlocal

echo ========================================
echo Component Registry Generator
echo ========================================

set SCRIPT_DIR=%~dp0
set PROJECT_DIR=%SCRIPT_DIR%..\..\
set PYTHON_EMBEDDED=%PROJECT_DIR%Engine\ThirdParty\Python\python.exe

REM 組み込み Python が存在するかチェック
if not exist "%PYTHON_EMBEDDED%" (
    echo.
    echo ERROR: Python が見つかりません
    echo Tools\Python\python.exe を配置してください
    pause
    exit /b 1
)

echo Python: %PYTHON_EMBEDDED%
echo.

REM 組み込み Python を使用
set PYTHONIOENCODING=shift-jis
"%PYTHON_EMBEDDED%" "%SCRIPT_DIR%GenerateComponentRegistry.py" --source-dir "%PROJECT_DIR%Engine" --output "%PROJECT_DIR%Engine\Core\System\Generated\ComponentRegistry_Generated.cpp"

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: Failed to generate component registry
    pause
    exit /b 1
)

echo.
echo ========================================
echo Generation Complete!
echo ========================================
pause
exit /b 0