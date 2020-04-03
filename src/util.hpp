#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <cstdio>
#include <vector>

template <typename T>
std::vector<T> operator+(const std::vector<T> &a, const std::vector<T> &b) {
  std::vector<T> newvector(a.size() + b.size());
  std::copy(a.begin(), a.end(), newvector.begin());
  std::copy(b.begin(), b.end(), newvector.begin() + a.size());
  return newvector;
}

// The USE(x) template is used to silence C++ compiler warnings
// issued for (yet) unused variables (typically parameters).
template <typename T>
inline void USE(T) {}

#define DEBUG 1
#define INFO 1
#define WARN 1
#define WAIT 0
#define PRINT_STACKS 1
#define FATAL(...)                                               \
  do {                                                           \
    fprintf(stderr, "Fatal Error(%s:%d): ", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__);                                \
    exit(1);                                                     \
  } while (0)

#define ASSERT(exp, ...)                                              \
  do {                                                                \
    if (!(exp)) {                                                     \
      fprintf(stderr, "Assert Failed (%s:%d): ", __FILE__, __LINE__); \
      fprintf(stderr, __VA_ARGS__);                                   \
      exit(1);                                                        \
    }                                                                 \
  } while (0)

/*
 * macros for printing
 */

#if DEBUG
#define debug(...)                                          \
  do {                                                      \
    fprintf(stderr, "DEBUG (%s:%d): ", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__);                           \
  } while (0)
#else
#define debug(...) \
  do {             \
  } while (0)
#endif

#if INFO
#define info(...)                                           \
  do {                                                      \
    fprintf(stderr, "INFO  (%s:%d): ", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__);                           \
  } while (0)
#else
#define info(...) \
  do {            \
  } while (0)
#endif

#if WARN
#define warn(...)                                           \
  do {                                                      \
    fprintf(stderr, "WARN  (%s:%d): ", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__);                           \
  } while (0)
#else
#define warn(...) \
  do {            \
  } while (0)
#endif

inline void tabs(int n) {
  for (int i = 0; i < n; i++) std::cout << "\t";
}
#define iloop(vec) for (int i = 0; i < vec.size(); i++)
#define printvec(vec, ntabs)             \
  for (int i = 0; i < vec.size(); i++) { \
    tabs(ntabs);                         \
    std::cout << vec[i] << std::endl;    \
  }

/*****************************************************************************/

#endif
