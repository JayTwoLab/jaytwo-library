@REM Set base install path (no surrounding quotes in value)
set "INSTALL_BASE=C:/mingw-install/j2_library"
set "INSTALL_PATH_DEBUG=%INSTALL_BASE%/debug"
set "INSTALL_PATH_RELEASE=%INSTALL_BASE%/release"

@REM Create build directories
mkdir build-debug
mkdir build-release

@REM Configure and build Debug
cmake -S .. -B build-debug -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug
cmake --install build-debug --prefix "%INSTALL_PATH_DEBUG%"

@REM Configure and build Release
cmake -S .. -B build-release -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build-release
cmake --install build-release --prefix "%INSTALL_PATH_RELEASE%"

@REM If using clang
@REM    cmake -S .. -B build-release -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang ...

@REM LLVM/Clang environment variables
@REM PATH      C:\llvm-package\bin
@REM LLVM_DIR  C:\llvm-package\lib\cmake\llvm
@REM Clang_DIR C:\llvm-package\lib\cmake\clang
