@REM Set base install path (no surrounding quotes in value)
set "INSTALL_PATH=C:/vs-install/j2_library"
set "INSTALL_PATH_DEBUG=%INSTALL_PATH%/debug"
set "INSTALL_PATH_RELEASE=%INSTALL_PATH%/release"

@REM Create build directory
mkdir build 
cd build

@REM Configure the project (generate project files)
cmake -S .. -B build -G "Visual Studio 17 2022" -A x64

@REM If an argument is provided, build/install only that config (Debug or Release).
if "%~1"=="" goto build_all
if /I "%~1"=="Debug" goto build_debug
if /I "%~1"=="Release" goto build_release
echo Invalid argument. Use Debug, Release, or no argument for both.
exit /b 1

:build_all
@REM Build and install Debug
cmake --build build --config Debug
cmake --install build --config Debug --prefix "%INSTALL_PATH_DEBUG%"

@REM Build and install Release
cmake --build build --config Release
cmake --install build --config Release --prefix "%INSTALL_PATH_RELEASE%"
goto :eof

:build_debug
cmake --build build --config Debug
cmake --install build --config Debug --prefix "%INSTALL_PATH_DEBUG%"
goto :eof

:build_release
cmake --build build --config Release
cmake --install build --config Release --prefix "%INSTALL_PATH_RELEASE%"
goto :eof

