@REM Remove the library directory if it exists before proceeding.
rmdir /s /q C:\opt\j2_library

@REM CMake configuration
@REM Library install path: C:/opt/j2_library
@REM Use Release build
cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="C:/opt/j2_library"

@REM Build
cmake --build build

@REM Install
cmake --install build
