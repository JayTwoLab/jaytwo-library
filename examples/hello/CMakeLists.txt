# hello/CMakeLists.txt
cmake_minimum_required(VERSION 3.26)

project(j2_hello
  VERSION 0.1.0
  LANGUAGES CXX
)

###############################################
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../j2_library/cmake")
#  CMAKE_CURRENT_SOURCE_DIR → 현재 처리 중인 CMakeLists.txt 파일이 위치한 디렉토리

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
option(USE_VCPKG "vcpkg를 사용합니다." ON) # 기본값 ON
if (USE_VCPKG)
  include(vcpkg OPTIONAL) # vcpkg.cmake
  setup_platform_defaults_with_vcpkg()
endif()
#################################################

add_executable(j2_hello
  main.cpp
)

target_link_libraries(j2_hello PRIVATE
  j2_library::j2_library # j2_library 라이브러리 링크
)

 
