@echo off
:: ============================================================
:: wlx-edge-viewer 构建脚本
:: 
:: 前置要求:
::   - MS Visual Studio 2022 (v143 toolset)
::   - vcpkg (已集成 MSBuild)
::   - Windows SDK 10.0
::
:: 用法:
::   build.bat          - 构建 Release 版本并打包
::   build.bat debug    - 构建 Debug 版本
::   build.bat clean    - 清理构建目录
:: ============================================================

setlocal enabledelayedexpansion

set CONFIG=Release
set ACTION=build

if "%1"=="debug" set CONFIG=Debug
if "%1"=="clean" set ACTION=clean

echo.
echo ========================================================
echo  wlx-edge-viewer Build Script
echo  Configuration: %CONFIG%
echo ========================================================
echo.

:: 检查 Visual Studio 环境
where msbuild >nul 2>&1
if errorlevel 1 (
    echo [ERROR] msbuild not found. Please run from Developer Command Prompt.
    echo         Or run: call "%VCINSTALLDIR%\Auxiliary\Build\vcvarsall.bat"
    exit /b 1
)

:: 清理
if "%ACTION%"=="clean" (
    echo [INFO] Cleaning build directories...
    if exist Build rmdir /S /Q Build
    echo [INFO] Clean complete.
    exit /b 0
)

:: 创建构建目录
if not exist Build mkdir Build

:: ============================================================
:: 编译 Win32 (32-bit)
:: ============================================================
echo.
echo [1/3] Building Win32 (%CONFIG%)...
echo.

msbuild EdgeViewer.sln /t:Build /p:Configuration=%CONFIG%;Platform=Win32;UseEnv=true /v:minimal
if errorlevel 1 (
    echo [ERROR] Win32 build failed!
    exit /b 1
)
echo [OK] Win32 build complete.

:: ============================================================
:: 编译 x64 (64-bit)
:: ============================================================
echo.
echo [2/3] Building x64 (%CONFIG%)...
echo.

:: Switch to x64 environment for x64 build
if not "%VSCMD_ARG_TGT_ARCH%"=="x64" (
    echo [INFO] Switching to x64 environment...
    call "%%VCINSTALLDIR%%\Auxiliary\Build\vcvarsall.bat" x64
)
msbuild EdgeViewer.sln /t:Build /p:Configuration=%CONFIG%;Platform=x64;UseEnv=true /v:minimal
if errorlevel 1 (
    echo [ERROR] x64 build failed!
    exit /b 1
)
echo [OK] x64 build complete.

:: ============================================================
:: 打包发布
:: ============================================================
echo.
echo [3/3] Creating release package...
echo.

set RELEASE_DIR=Build\Release
set DATE_STR=%date:~0,4%%date:~5,2%%date:~8,2%

:: 清理旧的发布目录
if exist %RELEASE_DIR% rmdir /S /Q %RELEASE_DIR%
mkdir %RELEASE_DIR%

:: 复制资源文件
echo [INFO] Copying resources...
xcopy /E /I /Q Resources\* %RELEASE_DIR% >nul

:: 复制编译产物
if "%CONFIG%"=="Release" (
    copy /Y Build\EdgeViewer_Win32_Release\EdgeViewer-Win32.dll %RELEASE_DIR%\EdgeViewer.wlx >nul
    copy /Y Build\EdgeViewer_x64_Release\EdgeViewer-x64.dll %RELEASE_DIR%\EdgeViewer.wlx64 >nul
) else (
    copy /Y Build\EdgeViewer_Win32_Debug\EdgeViewerD-Win32.dll %RELEASE_DIR%\EdgeViewer.wlx >nul
    copy /Y Build\EdgeViewer_x64_Debug\EdgeViewerD-x64.dll %RELEASE_DIR%\EdgeViewer.wlx64 >nul
)

:: 创建 ZIP 包
echo [INFO] Creating ZIP archive...
if exist Release-%DATE_STR%.zip del Release-%DATE_STR%.zip

powershell.exe -NoProfile -Command ^
    "Add-Type -AssemblyName System.IO.Compression.FileSystem; " ^
    "[System.IO.Compression.ZipFile]::CreateFromDirectory('%RELEASE_DIR%', 'Release-%DATE_STR%.zip')"

if errorlevel 1 (
    echo [ERROR] Failed to create ZIP!
    exit /b 1
)

:: ============================================================
:: 完成
:: ============================================================
echo.
echo ========================================================
echo  Build Complete!
echo.
echo  Release package: Release-%DATE_STR%.zip
echo  Release folder:  %RELEASE_DIR%
echo.
echo  Files:
dir /B %RELEASE_DIR%\EdgeViewer.wlx* 2>nul
echo.
echo ========================================================

endlocal
exit /b 0
