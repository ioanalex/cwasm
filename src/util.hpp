#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <cstdio>
#include <vector>

template <typename T>
std::vector<T> operator+(const std::vector<T> &a, const std::vector<T> &b) {
  std::vector<T> newvector(a.size() + b.size());
  std::copy(a.begin(), a.end(), newvector.begin());
  std::copy(b.begin(), b.end(), newvector.end() + a.size());
  return newvector;
}

// The USE(x) template is used to silence C++ compiler warnings
// issued for (yet) unused variables (typically parameters).
template <typename T>
inline void USE(T) {}

#define DEBUG 0
#define INFO 0
#define WARN 0
#define WAIT 0

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
#define itloop(vec) for (auto it = vec.begin(); it != vec.end(); it++)
#define printvec(vec, ntabs)         \
  itloop(vec) {                      \
    tabs(ntabs);                     \
    std::cout << (*it) << std::endl; \
  }

/*****************************************************************************/

#endif
