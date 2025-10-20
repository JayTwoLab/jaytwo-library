# Modern C++ 날짜 및 시간 라이브러리

## 개요

- C++17 이하 버전에서 사용할 수 있는 현대적인 날짜 및 시간 라이브러리입니다.
- C++17 이하 에서는 Howard tz 라이브러리를 사용하여 타임존을 처리하는 것도 유용됩니다.
    - https://github.com/HowardHinnant/date
    - Howard tz 는 향후 c++20 으로 이식하기 쉽도록 설계되었습니다.
- C++20 에서는 타임존을 사용할 수 있으며, 해당 클래스들을 사용하는 것이 권장됩니다.

## C++17 vs C++20 날짜·시간·타임존 비교

### C++17의 특징

- 시간 단위는 나노초, 마이크로초, 밀리초, 초, 분, 시간까지 지원.
- 시계는 `system_clock`, `steady_clock`, `high_resolution_clock`만 제공.
- 날짜·달력 클래스는 없어 `std::tm`과 `std::time_t` 같은 C 구조체에 의존.
- 지역 시간과 UTC 변환은 `std::localtime`, `std::gmtime` 같은 C API를 사용해야 하며, 스레드 안전하지 않음.
- 타임존 기능이 아예 없어 Howard Hinnant의 `date.h` 같은 외부 라이브러리에 의존해야 함.
- 윤초(Leap second) 처리 기능이 없음.
- 날짜와 시간의 포맷팅·파싱은 `strftime`, `strptime` 같은 C 라이브러리에 의존.

### C++20의 확장

- 기존 시계 외에 `utc_clock`, `tai_clock`, `gps_clock`이 추가됨.
- 날짜 표현을 위한 다양한 클래스가 새로 도입됨: `year`, `month`, `day`, 그리고 `year_month_day`, `year_month_weekday`, `weekday`, `month_day_last` 등
- 지역 시간과 UTC 변환이 강화되어 `sys_time`, `utc_time`, `local_time`, `zoned_time` 등을 통해 안전하게 변환 가능.
- 타임존을 다루는 `time_zone`, `time_zone_link`가 추가되고, IANA Time Zone Database(`tzdb`)가 내장되어 `locate_zone("Asia/Seoul")` 같은 방식으로 손쉽게 사용 가능.
- 윤초를 표현하는 `std::chrono::leap_second`가 추가됨.
- 출력과 파싱은 `std::format` 및 `std::chrono::format`을 통해 ISO 8601 형식 지원.                                                                      
                                                                      

 

---

- 👉 요약하면, **C++17은 “시간차(duration)” 계산 위주** 이고,
- **C++20부터는 “날짜/캘린더/타임존까지 완전 지원”** 한다고 보시면 됩니다.
