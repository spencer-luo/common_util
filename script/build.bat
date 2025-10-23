@echo off

@REM get arguments count
set /A "param_count=0"
for %%I in (%*) do (
  set /A "param_count+=1"
)

if %param_count% lss 1 (
    echo arguments error, usages:
    echo .\build.bat build [platform] [buildType]
    echo .\build.bat run
    echo .\build.bat clean
    echo .\build.bat pack
    exit 0
)

@REM 读取可执行程序的路径
if exist ./build/executable_filepath.data (
    set /p executable_filepath=<./build/executable_filepath.data    
)
@REM 执行common_util.exe
if %~1==run (
    if defined executable_filepath (
        @REM echo executable_filepath: %executable_filepath%
        %executable_filepath%
    )
    exit 0
)

if %~1==clean (
    @REM 参数说明：--%，停止解析符号，告诉PowerShell停止解析其余参数，并按原样传递它们
    @REM 参考文档：https://www.cnblogs.com/fireblackman/p/17323631.html
    cmd --% /c RMDIR /Q/S build
    echo ./build Cleaned
    exit 0
)

if %~1==pack (
    echo Pack package...
    cd build
    cpack
    echo pack package Done.
    exit 0
)


if %~1==build (
    @REM set platform0 and buildType0
    set platform0=%~2
    set buildType0=%~3
) else (
    echo arguments error, usages:
    echo .\build.bat build [platform] [buildType]
    echo .\build.bat run
    echo .\build.bat clean
    echo .\build.bat pack
    exit 0
)
if %~1==build (
    @REM verify platform0 and buildType0
    @REM echo platform0: %platform0%
    if "%platform0%"=="" (
        echo "platform is empty, usages:.\build.bat build [x86|x64] [release|rls|debug|dbg]"
        exit 0
    )
    if "%buildType0%"=="" (
        echo "buildType is empty, usages:.\build.bat build [x86|x64] [release|rls|debug|dbg]"
        exit 0
    )

    @REM echo build0 %platform0% %buildType0%
)

if %~1==build (
    @REM set platform and buildType
    if /i %buildType0% == Release (
        set buildType=Release
    ) else (
        set buildType=Debug
    )
    if /i %platform0% == X64 (
        set platform=x64
    ) else (
        set platform=win32
    )
)

@REM TODO: 指定cmake的生成器，用户可以替换成自己安装的对应Visual Studio的版本
@REM set generator="Visual Studio 14 2015"
set generator="Visual Studio 17 2022"
@REM TODO: 指定CPU核数，用户可以根据自己电脑的CPU核数来调整设置这个参数。使用的cpu核数越多，编译速度越快。
set cpu_cores=6

if %~1==build (
    @REM echo build %platform% %buildType%
    @REM echo generator: %generator%

    echo Building...
    @REM configure
    cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=%buildType% -S ./ -B ./build -G %generator% -A %platform% -DBUILD_DEMO=ON
    @REM build
    cmake --build ./build --config %buildType% --target ALL_BUILD -j %cpu_cores% --
)


@REM -DCMAKE_BUILD_TYPE:STRING=Debug

@REM release x86_64
@REM configure
@REM "C:\Program Files\CMake\bin\cmake.EXE" --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -SY:/common_util -By:/common_util/build -G "Visual Studio 14 2015" -A x64
@REM build
@REM "C:\Program Files\CMake\bin\cmake.EXE" --build y:/common_util/build --config Release --target ALL_BUILD -j 6 --

@REM release x86
@REM configure
@REM "C:\Program Files\CMake\bin\cmake.EXE" --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -SY:/common_util -By:/common_util/build -G "Visual Studio 14 2015" -A win32
@REM build
@REM "C:\Program Files\CMake\bin\cmake.EXE" --build y:/common_util/build --config Release --target ALL_BUILD -j 6 --
