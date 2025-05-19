@echo off
REM テスト実行用スクリプト
chcp 65001 > nul

set BUILD_TYPE=Debug
if not "%1"=="" set BUILD_TYPE=%1

set TEST_DIR=build\tests\%BUILD_TYPE%
set DLL_DIR=build\%BUILD_TYPE%

echo Running tests from %TEST_DIR%...

REM 必要なDLLをテストディレクトリにコピー
copy %DLL_DIR%\*.dll %TEST_DIR%\

REM テストを実行
cd %TEST_DIR%
tcp_server_test.exe

if errorlevel 1 (
  echo Tests failed!
) else (
  echo All tests passed!
)

cd ..\..\..
exit /b 