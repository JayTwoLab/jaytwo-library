## `j2_library` 라이브러리

<br />

---

### 0. `j2_library` 소개

- `j2_library`는 스위스 나이프처럼 활용되는 라이브러리입니다.
	- `C++` 표준과 써드 파티 일부 사용
	- `Windows`, `Linux` 지원 (`MacOS` 지원 예정)
	- `cmake`, `ninja` 빌드 시스템 사용

- 홈페이지 : https://github.com/JayTwoLab/jaytwo-library

- 개인적인 용도로 제작되었으며, 오픈 소스로 공개되어 있습니다.
	- [MIT 라이선스](LICENSE)

- 라이브러리 구성 개요
	- `bit`
		- 비트 연산 및 마스크 유틸리티 (flags, bit-manipulation)
	- `core`
		- 핵심 유틸리티 (error handling, result/optional, small helpers)
	- `datetime`
		- 날짜/시간 처리 및 포맷팅 유틸리티
	- `filesystem`
		- 파일 및 경로 조작, 디렉터리 유틸리티
	- `io`
		- 스트림, 파일 입출력 래퍼 및 헬퍼
	- `string`
		- 문자열 조작, 포맷팅, 인코딩 유틸리티
	- `math`
		- 수학 연산, 보조 함수, 고정소수점/수치 유틸리티
	- `container`
		- 확장 컨테이너 및 편의 알고리즘
	- `concurrency`
		- 스레드, 동기화 primitives, 작업 큐 유틸리티
	- `net`
		- 네트워크 유틸리티(소켓 래퍼 등) — 플랫폼별 추상화 포함
	- `serialization`
		- JSON 등 직렬화/역직렬화 도우미
	- `logging`
		- 로깅 래퍼 및 설정 유틸리티
	- `platform`
		- 플랫폼별 차이를 숨기는 추상화 레이어 (Windows/Linux)
	- `crypto`
		- 해시, 암호화/복호화 유틸리티(필요 시 외부 라이브러리 사용)
	- `test`
		- 테스트 유틸리티 및 GTest 헬퍼

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


