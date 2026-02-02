@echo off
setlocal

echo ========================================
echo Component Registry Generation
echo ========================================

set SCRIPT_DIR=%~dp0
set PROJECT_DIR=%SCRIPT_DIR%..\
set PYTHON_SCRIPT=%SCRIPT_DIR%GenerateComponentRegistry.py
set SOURCE_DIR=%PROJECT_DIR%Engine
set OUTPUT_FILE=%PROJECT_DIR%Engine%Assets%Generated\ComponentRegistry_Generated.cpp

echo.
echo Python Script: %PYTHON_SCRIPT%
echo Source Dir:    %SOURCE_DIR%
echo Output File:   %OUTPUT_FILE%
echo.

python "%PYTHON_SCRIPT%" --source-dir "%SOURCE_DIR%" --output "%OUTPUT_FILE%"

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
