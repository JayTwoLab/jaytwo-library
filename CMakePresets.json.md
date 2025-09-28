# 프리셋 `CMakePresets.json`

---

## `Visual Studio` 용 CMake 프리셋 예제

- `CMakePresets.json` 

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

---

