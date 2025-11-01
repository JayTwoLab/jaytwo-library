
@REM 빌드 경로 생성
mkdir build 
cd build

@REM cmake 실행 (릴리즈 빌드, 설치 경로 지정)
@REM  - 설치 경로에 따라 -DCMAKE_INSTALL_PREFIX 값을 변경하세요.
@REM  - 디버그/릴리즈 모드에 따라 -DCMAKE_BUILD_TYPE 값을 변경하세요.
cmake -S .. -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="C:/mingw-install/j2_library"
@REM clang 사용하는 경우
@REM    cmake -S .. -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang ... 

@REM LLVM/Clang 환경 변수 설정 
@REM PATH      C:\llvm-package\bin
@REM LLVM_DIR  C:\llvm-package\lib\cmake\llvm
@REM Clang_DIR C:\llvm-package\lib\cmake\clang

@REM 라이브러리 빌드
cmake --build build

@REM 라이브러리 설치
cmake --install build
