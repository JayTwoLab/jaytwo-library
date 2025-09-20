
## 프리셋 빌드

- `MingW` 빌드 케이스

```bat
cmake --list-presets
cmake --preset windows-mingw-release
cmake --build --preset windows-mingw-release -j
ctest --preset windows-mingw-release
```



