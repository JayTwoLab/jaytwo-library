cmake_minimum_required(VERSION 3.26)

project(j2_super
  VERSION 0.1.0
  LANGUAGES CXX
)



# -------------------------
# 전역 옵션
# -------------------------
option(BUILD_EXAMPLES "예제를 빌드합니다." ON)
option(BUILD_TESTING "테스트를 빌드합니다." ON)

# (선택) MinGW + vcpkg 스니펫: 사용 환경에 맞게 주석 해제하여 사용하십시오.
# if (WIN32 AND MINGW AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
#   set(VCPKG_ROOT $ENV{USERPROFILE})
#   set(CMAKE_PREFIX_PATH    "${VCPKG_ROOT}/vcpkg/installed/x64-mingw-static")
#   set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "" FORCE)
# endif()

###############################################
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
#  CMAKE_SOURCE_DIR → 항상 최상위 CMakeLists.txt 디렉토리 (프로젝트 루트)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
#  CMAKE_CURRENT_SOURCE_DIR → 현재 처리 중인 CMakeLists.txt 파일이 위치한 디렉토리

# 경고/샌리타이저 모듈(있으면 사용)
include(warnings OPTIONAL) # warnings.cmake
include(sanitize OPTIONAL) # sanitize.cmake

include(utf8 OPTIONAL) # utf8.cmake
##################################################

# -------------------------
# 라이브러리 서브프로젝트 포함
# -------------------------
add_subdirectory(j2_library)

# 상위에서 경고/샌리타이저를 라이브러리에 적용하고 싶다면 아래 주석 해제
# if (COMMAND j2_enable_warnings)
#   j2_enable_warnings(j2_library)
# endif()
# if (COMMAND j2_enable_sanitizers)
#   j2_enable_sanitizers(j2_library)
# endif()

# -------------------------
# 예제
# -------------------------
if (BUILD_EXAMPLES)
  add_subdirectory(examples/hello)
endif()

# -------------------------
# 테스트
# -------------------------
include(CTest)  # BUILD_TESTING 옵션 정의
if (BUILD_TESTING)
  add_subdirectory(tests)
endif()
