#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <cstdio>

#define DEBUG 1
#define INFO 1
#define WARN 1

#define FATAL(...)                                               \
  {                                                              \
    fprintf(stderr, "Fatal Error(%s:%d): ", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__);                                \
    exit(1);                                                     \
  }

#define ASSERT(exp, ...)                                              \
  {                                                                   \
    if (!(exp)) {                                                     \
      fprintf(stderr, "Assert Failed (%s:%d): ", __FILE__, __LINE__); \
      fprintf(stderr, __VA_ARGS__);                                   \
      exit(1);                                                        \
    }                                                                 \
  }

/*
 * macros for printing
 */

#if DEBUG
#define debug(...)                                          \
  {                                                         \
    fprintf(stderr, "DEBUG (%s:%d): ", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__);                           \
  }
#else
#define debug(...) ;
#endif

#if INFO
#define info(...)                                           \
  {                                                         \
    fprintf(stderr, "INFO  (%s:%d): ", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__);                           \
  }
#else
#define info(...) ;
#endif

#if WARN
#define warn(...)                                           \
  {                                                         \
    fprintf(stderr, "WARN  (%s:%d): ", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__);                           \
  }
#else
#define warn(...) ;
#endif

/*****************************************************************************/

#endif