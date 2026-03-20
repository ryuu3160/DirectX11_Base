@echo off
setlocal

REM %1 = EnginePath, %2 = ProjectDir, %3 = Configuration
set "ENGINE_PATH=%~1"
set "PROJECT_DIR=%~2"
set "CONFIG=%~3"

echo ENGINE_PATH=%ENGINE_PATH%
echo PROJECT_DIR=%PROJECT_DIR%
echo CONFIG=%CONFIG%

python "%ENGINE_PATH%\Engine\BuildScripts\GenerateComponentRegistry.py" --source-dir "%PROJECT_DIR%Assets\Scripts" --output "%PROJECT_DIR%Generated\ComponentRegistry_Game_Generated.cpp" --function-name "RegisterAllGameComponents" --registry-mode "callback" --export
if errorlevel 1 exit /b 1

exit /b 0