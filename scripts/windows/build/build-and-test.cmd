@echo off
REM ビルドとテスト実行を組み合わせたスクリプト
chcp 65001 > nul

set BUILD_TYPE=Debug
set VCPKG_ROOT=C:\vcpkg

if not "%1"=="" set BUILD_TYPE=%1
if not "%2"=="" set VCPKG_ROOT=%2

echo BUILD_TYPE=%BUILD_TYPE%
echo VCPKG_ROOT=%VCPKG_ROOT%

REM ビルド実行
call build-utf8.cmd %BUILD_TYPE% %VCPKG_ROOT%
if errorlevel 1 goto error

REM テスト実行
echo Running tests directly...
call ..\..\test\run-tests.cmd %BUILD_TYPE%
if errorlevel 1 goto test_fail

echo Build and tests successful!
goto end

:error
echo Build failed!
goto end

:test_fail
echo Tests failed!

:end
exit /b 