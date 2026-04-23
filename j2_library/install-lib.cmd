@REM Set initial install directory
set "INSTALL_DIR=C:/opt/j2_library"

@REM Remove the library directory if it exists before proceeding.
if exist "%INSTALL_DIR%" (
    rmdir /s /q "%INSTALL_DIR%"
)

@REM CMake configuration
@REM Library install path: %INSTALL_DIR%
@REM Use Release build
cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%"

@REM Build
cmake --build build

@REM Install
cmake --install build
