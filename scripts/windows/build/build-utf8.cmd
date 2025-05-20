@echo off
REM UTF-8対応ビルドスクリプト
chcp 65001 > nul

set BUILD_TYPE=Debug
set VCPKG_ROOT=C:\vcpkg

if not "%1"=="" set BUILD_TYPE=%1
if not "%2"=="" set VCPKG_ROOT=%2

echo BUILD_TYPE=%BUILD_TYPE%
echo VCPKG_ROOT=%VCPKG_ROOT%

if exist build (
  rmdir /s /q build
)

mkdir build
cd build

echo Running CMake...
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_BUILD_TYPE=%BUILD_TYPE%

if errorlevel 1 goto error

echo Building...
cmake --build . --config %BUILD_TYPE%

if errorlevel 1 goto error

echo Running tests...
ctest -C %BUILD_TYPE% --output-on-failure

if errorlevel 1 goto test_fail

echo Build successful!
goto end

:error
echo Build failed!
goto end

:test_fail
echo Tests failed!

:end
cd ..
exit /b 