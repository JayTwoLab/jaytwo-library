## `j2_library` 라이브러리

<br />

---

### `Visual Studio`

- `vcpkg` 설치
    - `git clone https://github.com/microsoft/vcpkg.git`
	- `cd vcpkg`
	- `.\bootstrap-vcpkg.bat`
	- `vcpkg` 설치 경로를 `PATH`에 추가
- `Google Test` 설치 : `vcpkg install gtest:x64-windows-static`

<br />

---

### `MingW` (`+ Qt Creator`)

- `vcpkg` 설치
    - `git clone https://github.com/microsoft/vcpkg.git`
	- `cd vcpkg`
	- `.\bootstrap-vcpkg.bat`
	- `vcpkg` 설치 경로를 `PATH`에 추가
- `Google Test` 설치 : `vcpkg install gtest:x64-mingw-static`

<br />

---

### `Linux`

- `Google Test` 설치
   - `Rocky/CentOS` : `sudo dnf install gtest gtest-devel -y`
   - `Ubuntu/Debian` : `sudo apt-get install libgtest-dev -y`
   - `Arch` : `sudo pacman -S gtest`

<br />

---

