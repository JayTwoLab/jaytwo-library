@REM Create build directory
mkdir build 
cd build

@REM Run cmake (Release build, specify install path)
@REM  - Change the -G and -A options according to your Visual Studio version.
@REM  - Change the -DCMAKE_INSTALL_PREFIX value according to your desired install path.
@REM  - Change the -DCMAKE_BUILD_TYPE value for Debug/Release mode.
cmake -S .. -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="C:/vs-install/j2_library"

@REM Build the library
cmake --build build --config Release

@REM Install the library
cmake --install build --config Release

