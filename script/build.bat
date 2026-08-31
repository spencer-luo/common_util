@echo off
setlocal EnableExtensions EnableDelayedExpansion

@REM 与 build.sh 对齐的入口：build / run / clean / pack / test
@REM Windows 额外接受可选平台 x86|x64（默认 x64），以传给 Visual Studio 生成器的 -A。

if "%~1"=="" goto :usage

set "jobs_num=%NUMBER_OF_PROCESSORS%"
if not defined jobs_num set "jobs_num=4"

if not defined CMAKE_GENERATOR set "CMAKE_GENERATOR=Visual Studio 17 2022"

if /i "%~1"=="build" goto :do_build
if /i "%~1"=="run" goto :do_run
if /i "%~1"=="clean" goto :do_clean
if /i "%~1"=="pack" goto :do_pack
if /i "%~1"=="test" goto :do_test
goto :usage

:usage
echo arguments error, usages:
echo %~nx0 build [release^|rls^|debug^|dbg] [x86^|x64]
echo %~nx0 run
echo %~nx0 clean
echo %~nx0 pack
echo %~nx0 test [filter]      build and run unit tests, filter is an optional regex of test names
echo %~nx0 test clean         remove the unit test build directory
echo.
echo environment variables:
echo   UT_CXX11=ON            build unit tests with C++11, the same standard as the release build
echo   CMAKE_GENERATOR        Visual Studio generator, default is "Visual Studio 17 2022"
exit /b 0

:do_build
set "type_arg=%~2"
set "plat_arg=%~3"

@REM 兼容旧用法：build [x86|x64] [release|rls|debug|dbg]
if /i "%type_arg%"=="x64" (
    set "plat_arg=x64"
    set "type_arg=%~3"
)
if /i "%type_arg%"=="x86" (
    set "plat_arg=x86"
    set "type_arg=%~3"
)
if /i "%type_arg%"=="win32" (
    set "plat_arg=x86"
    set "type_arg=%~3"
)

if /i "%type_arg%"=="release" goto :build_release
if /i "%type_arg%"=="rls" goto :build_release
if /i "%type_arg%"=="debug" goto :build_debug
if /i "%type_arg%"=="dbg" goto :build_debug
goto :usage

:build_release
echo %type_arg%
set "build_type=Release"
set "release_binary=ON"
goto :build_platform

:build_debug
echo %type_arg%
set "build_type=Debug"
set "release_binary=OFF"
goto :build_platform

:build_platform
if "%plat_arg%"=="" set "plat_arg=x64"
if /i "%plat_arg%"=="x64" (
    set "platform=x64"
) else if /i "%plat_arg%"=="x86" (
    set "platform=Win32"
) else if /i "%plat_arg%"=="win32" (
    set "platform=Win32"
) else (
    goto :usage
)

echo Building...
cmake -S ./ -B ./build -G "%CMAKE_GENERATOR%" -A %platform% -DCMAKE_BUILD_TYPE:STRING=%build_type% -DBUILD_DEMO=ON -DCXX_STANDARD_11=ON -DRELEASE_BINARY_LIBRARY=%release_binary%
if errorlevel 1 exit /b 1
cmake --build ./build --config %build_type% -j %jobs_num%
if errorlevel 1 exit /b 1
echo Build Done.
exit /b 0

:do_run
if not exist .\build\executable_filepath.data (
    echo executable_filepath.data not found, build the demo first.
    exit /b 1
)
set /p executable_filepath=<.\build\executable_filepath.data
echo %executable_filepath%
if not exist "%executable_filepath%" (
    echo executable not found: %executable_filepath%
    exit /b 1
)
"%executable_filepath%"
exit /b %errorlevel%

:do_clean
if exist .\build rd /s /q .\build
if exist .\publish rd /s /q .\publish
if exist .\build_ut rd /s /q .\build_ut
if exist .\build_ut11 rd /s /q .\build_ut11
echo ./build, ./publish, ./build_ut and ./build_ut11 Cleaned.
exit /b 0

:do_pack
if not exist .\build (
    echo build directory not found, run "%~nx0 build rls" first.
    exit /b 1
)
echo Pack package...
pushd build
cpack
set "pack_err=%errorlevel%"
popd
if not "%pack_err%"=="0" exit /b %pack_err%
echo pack package Done.
exit /b 0

:do_test
if /i "%~2"=="clean" (
    if exist .\build_ut rd /s /q .\build_ut
    if exist .\build_ut11 rd /s /q .\build_ut11
    echo ./build_ut and ./build_ut11 Cleaned.
    exit /b 0
)

set "ut_dir=build_ut"
set "cmake_args=-DBUILD_UNIT_TEST=ON"
if /i "%UT_CXX11%"=="ON" (
    set "ut_dir=build_ut11"
    set "cmake_args=-DBUILD_UNIT_TEST=ON -DCXX_STANDARD_11=ON"
)

echo Configuring unit tests in %ut_dir%...
cmake -S . -B %ut_dir% %cmake_args%
if errorlevel 1 exit /b 1
echo Building unit tests...
cmake --build %ut_dir% --config Release -j %jobs_num%
if errorlevel 1 exit /b 1

set "no_tests_opt="
ctest --help 2>nul | findstr /c:"--no-tests" >nul
if not errorlevel 1 set "no_tests_opt=--no-tests=error"

echo Running unit tests...
if not "%~2"=="" (
    ctest --test-dir %ut_dir% --output-on-failure -j %jobs_num% %no_tests_opt% -C Release -R "%~2"
) else (
    ctest --test-dir %ut_dir% --output-on-failure -j %jobs_num% %no_tests_opt% -C Release
)
exit /b %errorlevel%
