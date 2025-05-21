@echo on
REM build script for utf-8
chcp 65001 > nul

set BUILD_TYPE=Debug

if not "%1"=="" set BUILD_TYPE=%1
if not "%2"=="" set VCPKG_ROOT=%2

echo BUILD_TYPE=%BUILD_TYPE%
echo VCPKG_ROOT=%VCPKG_ROOT%

cd "%~dp0..\..\..\"

if exist build (
  rmdir /s /q build
)

mkdir build
cd build

echo Running CMake...
cmake .. -G"Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_BUILD_TYPE=%BUILD_TYPE%

if errorlevel 1 goto error

echo Building...
cmake --build . --config %BUILD_TYPE%

if errorlevel 1 goto error


echo Build successful!
goto end

:error
echo Build failed!
goto end

:end
cd ..
exit /b 