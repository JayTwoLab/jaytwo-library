
### 사전 준비 사항

---

#### `vcpkg` 활용

- `vcpkg` 를 설치하여 라이브러리를 사용하는 방법

<br />

---

##### `vcpkg` + `Visual Studio` 

- `Visual Studio` 에서 `vcpkg` 를 설치하여 라이브러리를 사용하는 방법

- `vcpkg` 설치
    - `Visual Studio` 설치본에 `vcpkg`가 있는 경우 삭제하고, 하단의 방법으로 새로 설치 권장 
    - `git clone https://github.com/microsoft/vcpkg.git`
	    - 보통 사용자 계정 폴더에 설치함 : `C:\Users\j2\vcpkg`
    - 명령을 실행하여 `vcpkg.exe`를 생성
	    - `cd vcpkg`
	    - `.\bootstrap-vcpkg.bat`
	- `vcpkg` 설치 경로를 윈도 횐경 변수 `PATH`에 추가할 것
	- `vcpkg` 설치 경로를 윈도 횐경 변수 `VCPKG_ROOT`에 추가할 것

- `vcpkg` 로 종속성있는 패키지 설치
	```
	vcpkg install ^
	 gtest:x64-windows-static ^
	 nlohmann-json:x64-windows-static ^
	 spdlog:x64-windows-static ^
     curl:x64-windows-static ^
	 openssl:x64-windows-static
	```

<br />

---

##### `MingW` + `vcpkg`

- `MingW` 에서 `vcpkg` 를 설치하여 라이브러리를 사용하는 방법

- `vcpkg` 설치
    - `git clone https://github.com/microsoft/vcpkg.git`
		- 보통 사용자 계정 폴더에 설치함 : `C:\Users\j2\vcpkg`
    - 명령을 실행하여 `vcpkg.exe`를 생성
	    - `cd vcpkg`
	    - `.\bootstrap-vcpkg.bat`
	- `vcpkg` 설치 경로를 `PATH`에 추가할 것
    - `vcpkg` 설치 경로를 윈도 횐경 변수 `VCPKG_ROOT`에 추가할 것

- `vcpkg` 로 종속성있는 패키지 설치
	```
	vcpkg install ^
	 gtest:x64-mingw-static ^
	 nlohmann-json:x64-mingw-static ^
	 spdlog:x64-mingw-static ^
	 curl:x64-mingw-static ^
     openssl:x64-mingw-static
	```
		
<br />

---

#### `vcpkg` 를 설치하지 않는 방식

##### `Linux` (`non-vcpkg`)

- 리눅스에서도 `vcpkg`로 설치 가능하지만, 패키지 관리자를 이용하는 것이 더 편리함.

<br />

 
###### `Rocky/CentOS`

- 설치 명령
```
sudo dnf -y install \
 gtest gtest-devel gmock gmock-devel \
 json json-devel \
 spdlog spdlog-devel \
 curl curl-devel
```


<br />

###### `Ubuntu/Debian`

- 설치 명령
```
sudo apt install \
 libgtest-dev \
 nlohmann-json3-dev \
 libspdlog-dev \
 libcurl4-openssl-dev libssl-dev libpsl-dev
```

- NOTICE: 우분투에서 설치 점검 필요


