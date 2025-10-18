cmake_minimum_required(VERSION 3.26)

# 최상위 프로젝트
project(j2_lib_super VERSION 0.1.0 LANGUAGES CXX)

# -------------------------
# j2_library 라이브러리 서브 프로젝트 
# -------------------------
add_subdirectory(j2_library) # j2_library/CMakeLists.txt

# -------------------------
# 예제 서브 프로젝트 
# -------------------------
option(BUILD_EXAMPLES "Build Example" ON) # 값을 OFF 설정하거나, cmake -DBUILD_EXAMPLES=OFF 로 하면 예제 빌드하지 않음.
if (BUILD_EXAMPLES)
  add_subdirectory(hello-template) # hello-template/CMakeLists.txt
endif()

# -------------------------
# 테스트(Google Test) 서브 프로젝트
# -------------------------
option(BUILD_TESTING "Build Tests" ON) # 값을 OFF 설정하거나, cmake -DBUILD_TESTING=OFF 로 하면 테스트 빌드하지 않음.
if (BUILD_TESTING)
  include(CTest)  # BUILD_TESTING 옵션 정의
  add_subdirectory(tests) # tests/CMakeLists.txt
endif()

