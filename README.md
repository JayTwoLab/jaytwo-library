## `j2_library` 라이브러리

<br />

### 1. `vcpkg` 설치

---

#### 1.1. `Visual Studio`

- `vcpkg` 설치
    - `git clone https://github.com/microsoft/vcpkg.git`
	    - 보통 사용자 계정 폴더에 설치함 : `C:\Users\j2\vcpkg`
    - 명령을 실행하여 `vcpkg.exe`를 생성
	    - `cd vcpkg`
	    - `.\bootstrap-vcpkg.bat`
	- `vcpkg` 설치 경로를 윈도 횐경 변수 `PATH`에 추가
	- `vcpkg` 설치 경로를 윈도 횐경 변수 `VCPKG_ROOT`에 추가
- `Google Test` 설치
    - `vcpkg install gtest:x64-windows-static --vcpkg-root=C:\Users\j2\vcpkg`
	   - 설치 시 vcpkg 경로를 지정하지 않으면 `CMake`에서 `vcpkg`를 찾지 못하는 경우가 있음.

<br />

---

#### 1.2. `MingW` (`+ Qt Creator`)

- `vcpkg` 설치
    - `git clone https://github.com/microsoft/vcpkg.git`
		- 보통 사용자 계정 폴더에 설치함 : `C:\Users\j2\vcpkg`
    - 명령을 실행하여 `vcpkg.exe`를 생성
	    - `cd vcpkg`
	    - `.\bootstrap-vcpkg.bat`
	- `vcpkg` 설치 경로를 `PATH`에 추가
    - `vcpkg` 설치 경로를 윈도 횐경 변수 `VCPKG_ROOT`에 추가 
- `Google Test` 설치
	- `vcpkg install gtest:x64-mingw-static --vcpkg-root=C:\Users\j2\vcpkg` 
	   - 설치 시 vcpkg 경로를 지정하지 않으면 `CMake`에서 `vcpkg`를 찾지 못하는 경우가 있음.
		
<br />

---

### 2. `non-vcpkg` 방식으로 설치

#### 2.1. `Linux` (non-vcpkg)

- 리눅스에서도 `vcpkg`로 설치 가능하지만, 패키지 관리자를 이용하는 것이 더 편리함.
- `Google Test` 설치
   - `Rocky/CentOS` : `sudo dnf install gtest gtest-devel -y`
   - `Ubuntu/Debian` : `sudo apt-get install libgtest-dev -y`
   - `Arch` : `sudo pacman -S gtest`

<br />

---

