

# 경고/샌리타이저 모듈(있으면 사용)
if (COMMAND j2_enable_warnings)
  j2_enable_warnings(j2_library) # warnings.cmake
endif()
if (COMMAND j2_enable_sanitizers)
  j2_enable_sanitizers(j2_library) # sanitize.cmake
endif()

# 소스코드 인코딩 설정 모듈(있으면 사용)
include(utf8 OPTIONAL) # utf8.cmake

# vcpkg 설정 적용
if (MSVC)
  option(USE_VCPKG "[MSVC] vcpkg를 사용합니다." ON) 
endif()
if (MINGW)
  option(USE_VCPKG "[MINGW] vcpkg를 사용합니다." ON) 
endif()
if (USE_VCPKG)
  include(vcpkg OPTIONAL) # vcpkg.cmake
  setup_platform_defaults_with_vcpkg()
endif()

