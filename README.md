## `j2_library` Library

---

### 개요

`j2_library`는 `C++17` 기반의 범용 유틸리티 라이브러리입니다. 
- `Windows`와 `Linux`를 우선 지원하며, 향후 `macOS` 지원이 예정되어 있습니다.
- 빌드는 [`CMake`](https://cmake.org/) [`Ninja`](https://github.com/ninja-build/ninja) 기반으로 수행됩니다.

<details>

<summary>주요 기능 모듈:</summary>

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

</details>

<br />

---

### 1. 사전 준비

- [사전 설치 항목](Pre-Preparation.md)

- [사전 준비 사항](Pre-Installation.md)

<br />

---

### 2. 빌드

- **빠른 시작**

<details>
	
<summary>1. 소스 클론</summary>

```bash
git clone https://github.com/JayTwoLab/jaytwo-library.git
cd jaytwo-library
```

</details>

<details>	

<summary>2. 빌드</summary>

```bash
mkdir -p build && cd build
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release ..
ninja
```

</details>

<details>
	
<summary>3. 테스트 실행</summary>

```bash
ninja test
```

</details>

- [라이브러리 빌드 및 설치](Build-Lib.md)
- [설치된 라이브러리 사용하기](Use-Installed-Lib.md)


<br />

---

### 프로젝트 정보
- [홈페이지](https://github.com/JayTwoLab/jaytwo-library)
- 문의사항은 [이슈](https://github.com/JayTwoLab/jaytwo-library/issues)를 남겨주세요. 

<br />

---

### 라이선스

본 프로젝트는 듀얼 라이선스(dual-licensing) 정책을 채택합니다. 사용 목적에 따라 아래 라이선스 중 하나를 선택하여 적용하십시오.

<details>
	
<summary>1) 무료(오픈소스) 사용 — GNU Lesser General Public License v3 (LGPLv3)</summary>

- 소스 코드의 복제, 배포, 수정 및 파생 저작물 작성이 허용됩니다.
- LGPLv3의 조항을 따르며, 라이브러리를 수정하거나 수정된 라이브러리를 포함하여 재배포하는 경우 해당 변경사항을 공개해야 할 수 있습니다. 자세한 내용은 https://www.gnu.org/licenses/lgpl-3.0.html 를 참조하십시오.

</details>

<details>
<summary>2) 상용 사용 — 상용 라이선스</summary>

- 상용 사용자는 프로젝트 소유자로부터 제공되는 별도의 상용 라이선스를 취득할 수 있습니다.
- 상용 라이선스는 명시적으로 소스 코드를 자유롭게 사용, 수정, 통합 및 재배포할 수 있는 권한을 부여합니다(상표 권리와는 별도).
- 상용 라이선스의 구체적 조건(예: 재배포 조건, 서브라이선스 허용 여부, 유지보수/지원 포함 여부, 비용 등)은 라이선스 계약서에 따릅니다. 상용 라이선스를 원하시면 문의처(`jaytwo@jaytwo.kr`)로 연락해 주세요.

</details>

<details>
	
<summary>공통 조항</summary>

- 본 소프트웨어는 "있는 그대로"(AS IS) 제공되며, 명시적이거나 암시적인 보증이 없습니다. 프로젝트 소유자는 상품성, 특정 목적 적합성 또는 제3자 권리 미침해에 대한 보증을 포함하되 이에 국한되지 않는 어떠한 보증도 하지 않습니다.
- 어떤 경우에도 프로젝트 소유자는 본 소프트웨어 사용으로 인해 발생한 직간접적, 특별한, 부수적, 결과적 손해(수익 손실, 업무 중단, 데이터 손상 등)에 대해 책임을 지지 않습니다.
- 본 라이선스 설명은 법률 자문을 대체하지 않습니다. 기업용 배포 또는 법적 효력이 중요한 상황에서는 변호사와 상의하시기 바랍니다.

</details>

<details>
	
<summary>추가 고지</summary>

- 제3자 구성 요소에 적용되는 라이선스가 있을 수 있으며, 해당 구성 요소의 라이선스가 본 프로젝트에 추가적 의무를 부과할 수 있습니다. 개별 파일의 `LICENSE` 파일 및 주석을 확인하시기 바랍니다.
- 상표 및 로고 사용은 별도의 승인 대상일 수 있습니다. 상표 관련 문의는 위 연락처로 해주십시오.

</details>

자세한 라이선스 원문은 레포지토리 루트의 `LICENSE` 파일을 참조하십시오.



