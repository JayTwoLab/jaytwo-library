
# 빌드 경로 생성
mkdir build 
cd build

# cmake 실행 (릴리즈 빌드, 설치 경로 지정)
#  - 설치 경로에 따라 -DCMAKE_INSTALL_PREFIX 값을 변경하세요.
#  - 디버그/릴리즈 모드에 따라 -DCMAKE_BUILD_TYPE 값을 변경하세요.
cmake -S .. -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="/home/j2/workspace/dev/lib"

# 라이브러리 빌드 (cpu 코어 수 만큼 병렬 빌드)
cmake --build build -j$(nproc)

# 라이브러리 설치 
cmake --install build
# 설치 경로가 /usr/local/... 등의 시스템 경로인 경우, 설치 시 sudo(root 권한) 필요함.
#  sudo cmake --install build
