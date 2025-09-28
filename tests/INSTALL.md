
## `Google Test``

### 리눅스 계열

- `Rocky`/`CentOS`/`Fedora` `Linux` 패키지 설치 : `sudo yum install -y gtest-devel` (`dnf`로도 설치 가능)
- `Ubunut`/Debian` `Linux` 패키지 설치 : `sudo apt install -y libgtest-dev`

### MingW 계열

- `MSYS` 패키지 설치 : `pacman -S mingw-w64-x86_64-gtest` (64비트)
- `MingW` 패키지 설치 : `vcpkg install gtest:x64-mingw-static` (`static`은 옵션)

### Visual Studio 계열

- `vcpkg` 패키지 설치 : `vcpkg install gtest` (`x86`, `x64`, `static` 옵션 가능)
	- 일반적인 64비트 비주얼 스튜디오인 경우 : `vcpkg install gtest:x64-windows`

### `CMake`로 빌드
- `CMake`로 빌드 : `cmake -S . -B build -DBUILD_GMOCK=ON -DCMAKE_BUILD_TYPE=Release`
- 빌드 폴더로 이동 : `cd build`
- 빌드 실행 : `cmake --build . --config Release`
- (선택) 테스트 실행 : `ctest --output-on-failure --config Release`
- (선택) 설치 실행 : `sudo cmake --install . --prefix /usr/local`
- (선택) 설치 확인 : `ls /usr/local/lib/libgtest*`

