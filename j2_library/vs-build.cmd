
@REM 빌드 경로 생성
mkdir build 
cd build

@REM cmake 실행 (릴리즈 빌드, 설치 경로 지정)
@REM  - 사용하는 Visual Studio 버전에 맞게 -G 옵션과 -A 옵션을 변경하세요.
@REM  - 설치 경로에 따라 -DCMAKE_INSTALL_PREFIX 값을 변경하세요.
@REM  - 디버그/릴리즈 모드에 따라 -DCMAKE_BUILD_TYPE 값을 변경하세요.
cmake -S .. -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="C:/vs-install/j2_library"

@REM 라이브러리 빌드 
cmake --build build --config Release

@REM 라이브러리 설치
cmake --install build --config Release

