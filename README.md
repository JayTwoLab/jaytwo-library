## `j2_library` Library

> :kr: This project only supports Korean.

<br />

---

### 0. `j2_library` 소개

- C++ 기반 다기능 라이브러리
	- `C++` 표준과 써드 파티 일부 사용
	- `Windows`, `Linux` 지원 (`MacOS` 지원 예정)
	- `cmake`, `ninja` 빌드 시스템 사용

- 라이브러리 구성 개요
	- `bit`
		- 비트 연산  
	- `core`
		- 일반 유틸리티 
	- `datetime`
		- 날짜/시간 처리 및 변환
    - `directory`
		- 디렉토리 생성 유틸
	- `encoding`
	    - 문자열 인코딩 변환
	- `expected`
		- C++23 `std::expected` 대체 유틸 
	- `file`
		- 파일 찾기
	- `ini`
		- INI 파일 읽기/쓰기
	- `json`
		- 놀먼 json 활용 유틸
	- `log`
		- 무정지 로거 관리자
	- `macro`
		- setter/getter 매크로
		- try-opt 매크로
	- `network`
		- HTTP/HTTPS 파일 다운로드
		- FTP/SFTP 클라이언트
		- REST API
		- TCP 클라이언트/서버
		- UDP 수신자/송신자
	- `overload`
		- 람다 상속 유틸리티
	- `queue`
		- SPSC 스레드 안전 큐
	- `schedule`
		- 주단위 스케쥴러
	- `string`
		- 뮤텍스 문자열
		- 크로스 플랫폼 콘솔 인코딩
		- float/double 문자열 변환 및 비교
		- UTF8 문자열
	- `thread`
		- 동적 쓰레드
	- `uuid`
		- UUID 생성
	- `xml`
		- 소형 XML 파서
		 
<br />

---

### 1. 사전 준비

- [사전 설치 항목](Pre-Preparation.md)
	- `C++` 컴파일러 설치
	- `CMake` 설치
	- `ninja` 설치

- [사전 준비 사항](Pre-Installation.md)
	- `vcpkg` + `Visual Studio`
	- `MingW` + `vcpkg`
	- `Linux` (`non-vcpkg`)

<br />

---

### 2. 빌드

- [라이브러리 빌드 및 설치](Build-Lib.md)
	- `Visual Studio` 
	- `MinGW` (`g++`) 
	- `Linux` (`g++`) 

- [설치된 라이브러리 사용하기](Use-Installed-Lib.md)
	- `Visual Studio` 
	- `MinGW` (`g++`) 
	- `Linux` (`g++`) 

<br />

---

### 프로젝트 정보
- 홈페이지 : https://github.com/JayTwoLab/jaytwo-library
	- 문의사항은 이슈를 남겨주세요
		- https://github.com/JayTwoLab/jaytwo-library/issues
	- [mail](mailto:jaytwo@jaytwo.kr)

<br />

---

### 라이선스 
- [MIT 라이선스](LICENSE) 



