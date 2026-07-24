@echo off
REM ==============================================================================
REM windows-toast-notifications-0.9.0 - Test Runner
REM ==============================================================================

setlocal enabledelayedexpansion

echo.
echo ========================================
echo windows-toast-notifications-0.9.0
echo Test Suite Runner
echo ========================================
echo.

cd /d "%~dp0"

REM Check if build directory exists
if not exist "build\tests\Release\" (
    echo [ERROR] Tests not built yet!
    echo.
    echo Please run build.bat first:
    echo   build.bat
    echo.
    exit /b 1
)

REM Check if test executables exist
if not exist "build\tests\Release\unit_tests.exe" (
    echo [ERROR] unit_tests.exe not found!
    echo Please rebuild with BUILD_TESTS=ON
    exit /b 1
)

if not exist "build\tests\Release\test_suite.exe" (
    echo [ERROR] test_suite.exe not found!
    echo Please rebuild with BUILD_TESTS=ON
    exit /b 1
)

echo.
echo ========================================
echo STEP 1: Unit Tests (Automated)
echo ========================================
echo.
echo Running automated unit tests...
echo No user interaction required.
echo.

cd build\tests\Release
unit_tests.exe
set UNIT_TEST_RESULT=%ERRORLEVEL%
cd ..\..\..

if %UNIT_TEST_RESULT% NEQ 0 (
    echo.
    echo [ERROR] Unit tests failed!
    echo Please review the output above.
    echo.
    exit /b %UNIT_TEST_RESULT%
)

echo.
echo ========================================
echo Unit Tests: ✅ PASSED
echo ========================================
echo.
echo.
echo ========================================
echo STEP 2: Integration Tests (Manual)
echo ========================================
echo.
echo This test suite requires user interaction:
echo   - You will need to click buttons
echo   - Type text in notifications
echo   - Dismiss toasts manually
echo.
echo The tests will guide you through each step.
echo.
echo Press Enter to start integration tests...
echo (or Ctrl+C to skip)
echo.
pause >nul

cd build\tests\Release
test_suite.exe
set INTEGRATION_TEST_RESULT=%ERRORLEVEL%
cd ..\..\..

echo.
echo ========================================
echo TEST RESULTS SUMMARY
echo ========================================
echo.

if %UNIT_TEST_RESULT% EQU 0 (
    echo ✅ Unit Tests:        PASSED
) else (
    echo ❌ Unit Tests:        FAILED
)

if %INTEGRATION_TEST_RESULT% EQU 0 (
    echo ✅ Integration Tests: PASSED
) else (
    echo ❌ Integration Tests: FAILED
)

echo.
echo ========================================
echo.

if %UNIT_TEST_RESULT% EQU 0 if %INTEGRATION_TEST_RESULT% EQU 0 (
    echo ✅ ALL TESTS PASSED!
    echo.
    echo The library is validated and ready for production.
    echo.
    echo Next steps:
    echo   1. Integrate into SinfonAI
    echo   2. Add music player notifications
    echo   3. Test with Qt integration
    echo.
    exit /b 0
) else (
    echo ❌ SOME TESTS FAILED!
    echo.
    echo Please review the test output and fix issues.
    echo.
    echo For help, see: tests\README.md
    echo.
    exit /b 1
)
