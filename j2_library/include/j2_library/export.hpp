#pragma once

// Static/Shared Separation: Empty export/visibility macros during static build.
#if defined(J2LIB_STATIC)
  #define J2LIB_API
  #define J2LIB_HIDDEN
#elif defined(_WIN32) || defined(__CYGWIN__)
  #if defined(J2LIB_BUILDING_DLL)
    #define J2LIB_API __declspec(dllexport)
  #else
    #define J2LIB_API __declspec(dllimport)
  #endif
  #define J2LIB_HIDDEN
#else
  #if __GNUC__ >= 4
    #define J2LIB_API __attribute__((visibility("default")))
    #define J2LIB_HIDDEN __attribute__((visibility("hidden")))
  #else
    #define J2LIB_API
    #define J2LIB_HIDDEN
  #endif
#endif
