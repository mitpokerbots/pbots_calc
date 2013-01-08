#ifndef __PBOTS_CALC_H__
#define __PBOTS_CALC_H__

#define VERBOSE

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
  #define EXPORT_DLL_FLAG __declspec(dllexport)
  #define STRTOK strtok_s
#else
  #define EXPORT_DLL_FLAG
  #define STRTOK strtok_r
#endif

#ifdef __cplusplus    // If used by C++ code,
extern "C" {          // we need to export the C interface
#endif

typedef struct {
  double* ev;
  char** hands;
  int iters;
  int size;
  int MC; // did we run Monte Carlo simulation?
} Results;

EXPORT_DLL_FLAG Results* alloc_results(void);
EXPORT_DLL_FLAG void print_results(Results*);
EXPORT_DLL_FLAG void free_results(Results*);
EXPORT_DLL_FLAG int calc(const char*, char*, char*, int, Results*);

#ifdef __cplusplus
}
#endif

#endif
