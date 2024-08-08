#pragma once
#ifdef EXPORT
    #ifdef MU_WINDOWS
        #define IMPEXP __declspec(dllexport)
    #else
        #define IMPEXP __attribute__((visibility("default")))
    #endif
#else
    #ifdef MU_WINDOWS
        #define IMPEXP __declspec(dllimport)
    #else
        #define IMPEXP
    #endif
#endif
