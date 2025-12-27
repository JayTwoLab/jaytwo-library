
@REM 기존에 파일이 있을 수 있으므로 라이브러리 경로를 지우고 진행한다. 
rmdir /s /q C:\opt\j2_library

@REM cmake 설정
@REM 라이브러리 설치 경로 : C:/opt/j2_library
@REM 라이브러리는 릴리즈 빌드
cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="C:/opt/j2_library"


@REM 빌드
cmake --build build

@REM 설치
cmake --install build
