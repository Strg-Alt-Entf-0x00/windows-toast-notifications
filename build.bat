@echo off
REM ==============================================================================
REM windows-toast-notifications-0.9.0 Build Script
REM ==============================================================================

setlocal enabledelayedexpansion

echo.
echo ========================================
echo windows-toast-notifications-0.9.0
echo ========================================
echo.

REM Change to script directory
cd /d "%~dp0"

REM Create build and logs directories
if not exist build mkdir build
if not exist logs mkdir logs

REM Timestamp for log file
for /f %%i in ('powershell -NoProfile -Command "Get-Date -Format yyyy-MM-dd_HH-mm-ss"') do set TS=%%i
set LOG=logs\%TS%_build.log

REM Check for Visual Studio 2022 (BuildTools first, then Community/Professional)
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VCVARSALL=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VCVARSALL=C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VCVARSALL=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VCVARSALL=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat"
) else (
    echo [ERROR] Visual Studio 2022 not found!
    echo Please install Visual Studio 2022 with C++ Desktop Development workload.
    exit /b 1
)

echo [1/4] Initializing Visual Studio environment...
call "%VCVARSALL%" x64 >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Failed to initialize Visual Studio environment
    exit /b 1
)
echo [OK] Visual Studio 2022 environment ready

echo.
echo [2/4] Running CMake configuration...
echo [LOG] %LOG%
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=ON > "..\%LOG%" 2>&1
if errorlevel 1 (
    echo [ERROR] CMake configuration failed. Check %LOG%
    cd ..
    exit /b 1
)
echo [OK] CMake configuration complete

echo.
echo [3/4] Building library and examples...
cmake --build . --config Release >> "..\%LOG%" 2>&1
if errorlevel 1 (
    echo [ERROR] Build failed. Check %LOG%
    cd ..
    exit /b 1
)
echo [OK] Build successful

cd ..

echo.
echo [4/4] Build complete!
echo [LOG] %LOG%
echo.
echo ========================================
echo Output:
echo   Library:  build\Release\windows-toast-notifications.lib
echo   Examples: build\examples\Release\basic_toast.exe
echo             build\examples\Release\advanced_toast.exe
echo ========================================
echo.

exit /b 0
