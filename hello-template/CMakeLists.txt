# hello-template/CMakeLists.txt

cmake_minimum_required(VERSION 3.26)

project(j2_hello_template
  VERSION 1.0.0
  LANGUAGES CXX
)

# [쉽게 사용하는 방법] j2_cmake 모듈 직접 포함
#  CMAKE_CURRENT_SOURCE_DIR → 현재 처리 중인 CMakeLists.txt 파일이 위치한 디렉토리
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../j2_library/cmake") 
include(j2_cmake OPTIONAL RESULT_VARIABLE _J2_CMAKE_FOUND)
if(NOT _J2_CMAKE_FOUND)
  message(FATAL_ERROR "Mandatory module j2_cmake.cmake not found, please check CMAKE_MODULE_PATH settings.")
endif()

# 실행 프로그램 설정
add_executable(${PROJECT_NAME}
  main.cpp
)


# j2_library 인클루드 경로
target_include_directories(j2_hello PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/../j2_library/include"
)

# NOTICE: version.hpp 파일은 j2_library 프로젝트에서 자동 생성됨. 해당 경로도 인클루드 경로에 추가해야 함.

#   $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/../../out/build/x64-Debug/j2_library/generated/j2_library>
#   $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/../../out/build/x64-Release/j2_library/generated/j2_library>

# ${CMAKE_SOURCE_DIR}/build/j2_library/generated/j2_library/version.hpp 

# j2_library 라이브러리 링크 경로
if(MSVC)
    target_link_directories(j2_hello PRIVATE
      $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/../out/build/x64-Debug/j2_library>
      $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/../j2_library/out/build/x64-Debug>
      $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/../out/build/x64-Release/j2_library>
      $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/../j2_library/out/build/x64-Release>
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
      # Lib Dir #1 (최상위 프로젝트 빌드 폴더)
      set(LIB_RELATIVE_PATH_1 "${CMAKE_CURRENT_SOURCE_DIR}/../out/build/x64-Debug/j2_library")
      get_filename_component(LIB_ABSOLUTE_PATH_1 "${LIB_RELATIVE_PATH_1}" ABSOLUTE)
      message(STATUS "Resolved absolute library path = ${LIB_ABSOLUTE_PATH_1}")
      # Lib Dir #2
      set(LIB_RELATIVE_PATH_2 "${CMAKE_CURRENT_SOURCE_DIR}/../j2_library/out/build/x64-Debug")
      get_filename_component(LIB_ABSOLUTE_PATH_2 "${LIB_RELATIVE_PATH_2}" ABSOLUTE)
      message(STATUS "Resolved absolute library path = ${LIB_ABSOLUTE_PATH_2}")
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
      # Lib Dir #3
      set(LIB_RELATIVE_PATH_3 "${CMAKE_CURRENT_SOURCE_DIR}/../out/build/x64-Release/j2_library")
      get_filename_component(LIB_ABSOLUTE_PATH_3 "${LIB_RELATIVE_PATH_3}" ABSOLUTE)
      message(STATUS "Resolved absolute library path = ${LIB_ABSOLUTE_PATH_3}")
      # Lib Dir #4
      set(LIB_RELATIVE_PATH_4 "${CMAKE_CURRENT_SOURCE_DIR}/../j2_library/out/build/x64-Release")
      get_filename_component(LIB_ABSOLUTE_PATH_4 "${LIB_RELATIVE_PATH_4}" ABSOLUTE)
      message(STATUS "Resolved absolute library path = ${LIB_ABSOLUTE_PATH_4}")
    endif()

elseif(MINGW)
    target_link_directories(j2_hello PRIVATE
        ${CMAKE_SOURCE_DIR}/build/j2_library
        ${CMAKE_CURRENT_SOURCE_DIR}/../j2_library/build
    )

    # Lib Dir #1
    set(LIB_RELATIVE_PATH_1 "${CMAKE_SOURCE_DIR}/build/j2_library")
    get_filename_component(LIB_ABSOLUTE_PATH_1 "${LIB_RELATIVE_PATH_1}" ABSOLUTE)
    message(STATUS "Resolved absolute library path = ${LIB_ABSOLUTE_PATH_1}")
    # Lib Dir #2
    set(LIB_RELATIVE_PATH_2 "${CMAKE_CURRENT_SOURCE_DIR}/../j2_library/build")
    get_filename_component(LIB_ABSOLUTE_PATH_2 "${LIB_RELATIVE_PATH_2}" ABSOLUTE)
    message(STATUS "Resolved absolute library path = ${LIB_ABSOLUTE_PATH_2}")

else() # Linux gcc
    target_link_directories(j2_hello PRIVATE
        ${CMAKE_SOURCE_DIR}/build/j2_library
        ${CMAKE_CURRENT_SOURCE_DIR}/../j2_library/build
    )

    # Lib Dir #1
    set(LIB_RELATIVE_PATH_1 "${CMAKE_SOURCE_DIR}/build/j2_library")
    get_filename_component(LIB_ABSOLUTE_PATH_1 "${LIB_RELATIVE_PATH_1}" ABSOLUTE)
    message(STATUS "Resolved absolute library path = ${LIB_ABSOLUTE_PATH_1}")
    # Lib Dir #2
    set(LIB_RELATIVE_PATH_2 "${CMAKE_CURRENT_SOURCE_DIR}/../j2_library/build")
    get_filename_component(LIB_ABSOLUTE_PATH_2 "${LIB_RELATIVE_PATH_2}" ABSOLUTE)
    message(STATUS "Resolved absolute library path = ${LIB_ABSOLUTE_PATH_2}")

endif()

# j2_library 라이브러리 링크
target_link_libraries(${PROJECT_NAME} PRIVATE
  j2_library  # j2_library::j2_library 
)


 
