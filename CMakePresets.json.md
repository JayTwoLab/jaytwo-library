# 프리셋 `CMakePresets.json`

---

## `Visual Studio` 용 `CMake`` 프리셋 예제

- 파일명: `CMakePresets.json` 

```json
{
  "version": 3,
  "cmakeMinimumRequired": {
    "major": 3,
    "minor": 23
  },
  "configurePresets": [
    {
      "name": "x64-debug",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/x64-debug",
      "cacheVariables": {
        "CMAKE_TOOLCHAIN_FILE": "C:/Users/j2dol/vcpkg/scripts/buildsystems/vcpkg.cmake",
        "CMAKE_BUILD_TYPE": "Debug"
      }
    },
    {
      "name": "x64-release",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/x64-release",
      "cacheVariables": {
        "CMAKE_TOOLCHAIN_FILE": "C:/Users/j2dol/vcpkg/scripts/buildsystems/vcpkg.cmake",
        "CMAKE_BUILD_TYPE": "Release"
      }
    }
  ]
}
```

- `CMAKE_TOOLCHAIN_FILE` 에 `vcpkg` 경로를 지정하여 `vcpkg` 패키지 매니저를 사용하도록 설정.
- `ninja` 빌드 시스템을 사용하도록 설정.
   - `ninja`가 없는 경우 설치 필요. (`scoop` 이나 `Visual Studio` 설치시 선택 가능)

---

