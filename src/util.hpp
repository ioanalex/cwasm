#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <cstdio>
#include <iostream>
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

#define DEBUG 0
#define INFO 1
#define WARN 1
#define WAIT 0
#define PRINT_STACKS 0
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

class indent {
public:
  explicit indent(int n, char c = '\t') : depth(n), prefix(c) {}
  friend std::ostream &operator<<(std::ostream &out, const indent &ind) {
    for (int i = 0; i < ind.depth; ++i) out << ind.prefix;
    return out;
  }

private:
  int depth;
  char prefix;
};

#define iloop(vec) for (unsigned i = 0; i < vec.size(); ++i)

template <typename T>
class printvec {
public:
  explicit printvec(const std::vector<T> &v, int n, char c = '\t')
      : vec(v), ind(n, c) {}
  friend std::ostream &operator<<(std::ostream &out, const printvec &pv) {
    for (const auto &t : pv.vec) out << pv.ind << t << std::endl;
    return out;
  }

private:
  const std::vector<T> &vec;
  indent ind;
};

/*****************************************************************************/

#endif
