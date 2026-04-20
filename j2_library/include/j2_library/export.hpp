#pragma once

// j2_library는 정적 라이브러리(.a / .lib) 전용으로 빌드됩니다.
// 심볼 export/import 및 visibility 어트리뷰트는 정적 빌드에서는 필요하지 않습니다.
// 빈 매크로를 유지하면 소스의 API 어노테이션을 변경할 필요 없이
// 향후 공유 라이브러리 지원 시 쉽게 복원할 수 있습니다.

#define J2LIB_API
#define J2LIB_HIDDEN

