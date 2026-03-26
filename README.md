## `j2_library` Library

---

### 개요

`j2_library`는 C++17 기반의 범용 유틸리티 라이브러리입니다. Windows와 Linux를 우선 지원하며, 향후 macOS 지원이 예정되어 있습니다. 빌드는 `CMake`(`Ninja`) 기반으로 수행됩니다.

주요 기능 모듈:
- `bit` — 비트 연산, 비트 배열 등
- `config` — 설정 파일 파싱/관리
- `core` — 범용 유틸리티, 단일 함수 래퍼 등
- `crypt` — 암호화/복호화, 암호 유틸
- `datetime` — 날짜/시간 처리, 변환, 문자열화
- `directory` — 디렉토리 생성/조작 유틸
- `encoding` — 문자열 인코딩 변환, 인코딩 함수
- `expected` — `std::expected` 대체 유틸
- `file` — 파일 탐색, 파일 정보, 실행 파일 경로 등
- `ini` — INI 파일 읽기/쓰기, 파서
- `json` — JSON 처리 유틸 (nlohmann/json 등 활용)
- `log` — 로거 관리자, 로그 유틸
- `macro` — 편의 매크로, getter/setter, try_opt 등
- `network` — HTTP/HTTPS/FTP, TCP/UDP, 네트워크 인터페이스 등
- `overload` — 람다 오버로드 유틸
- `queue` — SPSC 스레드 안전 큐, concurrent queue
- `result` — 함수 반환 결과 유틸
- `schedule` — 스케줄러, 주간 반복 등
- `string` — 문자열 유틸, UTF-8, 콘솔 인코딩, 뮤텍스 문자열 등
- `system` — 시스템 정보, 크래시 핸들러, 디바이스 ID, 리소스 모니터 등
- `thread` — 동적 스레드, 스레드 유틸
- `uuid` — UUID 생성, v4 지원
- `xml` — 경량 XML 파서, XML 파싱 유틸

<br />

---

### 1. 사전 준비

- [사전 설치 항목](Pre-Preparation.md)

- [사전 준비 사항](Pre-Installation.md)

<br />

---

### 2. 빌드

- **빠른 시작**
	1. 소스 클론
		```bash
		git clone https://github.com/JayTwoLab/jaytwo-library.git
		cd jaytwo-library
		```
	2. 빌드 (예: Release, out-of-source)
		```bash
		mkdir -p build && cd build
		cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release ..
		ninja
		```
	3. 테스트 실행
		```bash
		ninja test
		```

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
- 본 프로젝트는 MIT 라이선스로 배포됩니다. 자세한 내용은 `LICENSE` 파일을 참조하세요.



