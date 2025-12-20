
### 사전 설치 항목

<br />

---

#### `C++` 컴파일러 설치

- `Visual Studio` (`Windows`)
	- [Visual Studio 다운로드](https://visualstudio.microsoft.com/ko/downloads/)
- `MingW` (+ `gcc`) (`Windows`)
	- 권장: `Qt Creator`에서 `MingW`를 사용할 경우 [Qt 다운로드](https://www.qt.io/download) (오픈소스 버전)
	- [MingW 다운로드](http://mingw-w64.org/doku.php/download) 직접 설치도 가능 
- `Linux gcc`
	- Ubuntu/Debian
		- `sudo apt-get install build-essential`
	- Rocky/CentOS
		- `sudo dnf update`
		- `sudo dnf install epel-release`
		- `sudo dnf groupinstall "Development Tools"`
	- Arch
		- `sudo pacman -S base-devel`
- ⚠️ NOTICE: `clang` 에 대한 테스트는 하지 않음

<br />

---

#### `CMake` 설치

- [CMake 다운로드](https://cmake.org/download/) 직접 설치
- 윈도우 scoop, chocolatey, winget 등 패키지 관리자를 이용하여 설치 가능
	- scoop : `scoop install cmake`	
- 리눅스 패키지 관리자를 이용하여 설치 가능
	- Ubuntu/Debian : `sudo apt-get install cmake`
	- Rocky/CentOS : `sudo dnf install cmake` 또는 `sudo yum install cmake`
	- Arch : `sudo pacman -S cmake`

<br />

---

#### `ninja` 설치 

- [Ninja 다운로드](https://ninja-build.org/)
	- 윈도우: `scoop`, `chocolatey`, `winget` 등 패키지 관리자를 이용하여 설치 가능
- `CMake`에서 `Ninja`를 빌드 도구로 사용할 경우 설치
- `IDE` 설치
- `Windows`
	- `Visual Studio`
	- `Qt Creator` (+ `MingW`)
	- `VSCode` (+ `MingW` or `MSVC`)
- `Linux`
	- `VSCode` (+ `gcc` or `WSL`)

