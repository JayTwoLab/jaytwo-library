# nlohmann json 설치 방법

---

## (1) vcpkg 로 설치하는 방법
```
vcpkg install nlohmann-json:x64-windows (Visual Studio 64bit, 동적 링크 기준)
vcpkg install nlohmann-json:x64-windows-static (Visual Studio 64bit, 동적 링크 기준)
vcpkg install nlohmann-json:x64-mingw-static (MingW-w64 64bit, 정적 링크 기준)
vcpkg install nlohmann-json:x64-linux (Linux 64bit, 동적 링크 기준)
```

---

## (2) 리눅스 패키지로 설치하는 방법

### 1) Rockey 리눅스, CentOS, RHEL 계열 (EPEL 필요)
```bash
sudo dnf install epel-release 
sudo dnf install nlohmann-json-devel
```

---

### 2) Ubuntu, Debian 계열
```bash
sudo apt update
sudo apt install nlohmann-json3-dev (Ubuntu 22.04 이상)
sudo apt install nlohmann-json-dev (Ubuntu 20.04 / 18.04) 
```

---

## (3) 소스코드로 설치
```bash
git clone https ://github.com/nlohmann/json.git
cd json
mkdir build
cd build
cmake ..
sudo cmake --install .
```

---
## (4) `git submodule` 설정을 이용하여 다운로드 하는 방법

- `git clone` 시 서브모듈까지 한 번에 받으려면 `--recurse-submodules` 옵션을 사용
```bash
# git clone --recurse-submodules https://github.com/사용할-레포.git

git clone --recurse-submodules https://github.com/JayTwoLab/jaytwo-library.git
```

- `git clone`을 한 상태라면, 서브모듈 초기화와 업데이트
```bash
# git clone https://github.com/JayTwoLab/jaytwo-library.git 한 이후

git submodule update --init --recursive
```

