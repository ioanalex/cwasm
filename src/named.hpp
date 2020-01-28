#ifndef __NAMED_HPP__
#define __NAMED_HPP__

#include <utility>

// Adapted from https://github.com/joboccara/namedtype

template <typename T, typename Phantom>
class Named {
public:
  explicit Named(const T& value) : value_(value) {}
  explicit Named(T&& value) : value_(std::move(value)) {}
  operator T&() { return value_; }
  operator const T&() const { return value_; }
  T to_value() { return value_; }

private:
  T value_;
};

#endif
