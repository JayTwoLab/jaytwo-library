cmake_minimum_required(VERSION 3.26)

# 최상위 프로젝트
project(j2_lib_super
  VERSION 0.1.0
  LANGUAGES CXX
) 

# -------------------------
# 라이브러리 서브 프로젝트 
# -------------------------
add_subdirectory(j2_library) # j2_library/CMakeLists.txt

# -------------------------
# 예제 서브 프로젝트 
# -------------------------
option(BUILD_EXAMPLES "예제를 빌드합니다." ON) # 기본값 ON
# cmake -DBUILD_EXAMPLES=OFF 를 하면 예제 빌드하지 않음.
if (BUILD_EXAMPLES)
  add_subdirectory(examples/hello) # examples/hello/CMakeLists.txt
endif()

# -------------------------
# 테스트(google test) 서브 프로젝트
# -------------------------
option(BUILD_TESTING "테스트를 빌드합니다." ON) # 기본값 ON
# cmake -DBUILD_TESTING=OFF 를 하면 테스트 빌드하지 않음.
if (BUILD_TESTING)
  include(CTest)  # BUILD_TESTING 옵션 정의
  add_subdirectory(tests) # tests/CMakeLists.txt
endif()

