@echo on
REM build and test script
chcp 65001 > nul

set BUILD_TYPE=Debug
set VCPKG_ROOT=C:\vcpkg

if not "%1"=="" set BUILD_TYPE=%1
if not "%2"=="" set VCPKG_ROOT=%2

echo BUILD_TYPE=%BUILD_TYPE%
echo VCPKG_ROOT=%VCPKG_ROOT%

REM build start
call "%~dp0build-utf8.cmd" %BUILD_TYPE% %VCPKG_ROOT%
if errorlevel 1 goto error

REM  test start
echo Running tests directly...
call "%~dp0..\test\run-tests.cmd" %BUILD_TYPE%
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