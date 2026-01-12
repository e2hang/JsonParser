#ifndef JSON_CONFIG_H_
#define JSON_CONFIG_H_

#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef JSON_LIBRARY_EXPORT // compile define macro
        #define JSON_API __declspec(dllexport)
    #else
        #define JSON_API __declspec(dllimport)
    #endif
#else
    #define JSON_API __attribute__((visibility("default")))
#endif


#endif
