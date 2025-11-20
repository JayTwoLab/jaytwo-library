@REM Create build directory
mkdir build 
cd build

@REM Run cmake (Release build, specify install path)
@REM  - Change the -DCMAKE_INSTALL_PREFIX value according to your desired install path.
@REM  - Change the -DCMAKE_BUILD_TYPE value for Debug/Release mode.
cmake -S .. -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="C:/mingw-install/j2_library"
@REM If using clang
@REM    cmake -S .. -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang ... 

@REM LLVM/Clang environment variables
@REM PATH      C:\llvm-package\bin
@REM LLVM_DIR  C:\llvm-package\lib\cmake\llvm
@REM Clang_DIR C:\llvm-package\lib\cmake\clang

@REM Build the library
cmake --build build

@REM Install the library
cmake --install build
