#pragma once

/// Functions to selectively cast only if the given types are not of identical
/// size, for casting platform-specific ints only in cases where they're not
/// identical, to simultaneously avoid compiler warnings about both implicit
/// casts and unnecessary casts

#include <type_traits>

template<typename Tdest, typename Tsrc>
inline typename std::enable_if_t<sizeof(Tsrc) == sizeof(Tdest), Tdest> constexpr cast_if_required(Tsrc value) __attribute__((__const__)) __attribute__((__always_inline__));
template<typename Tdest, typename Tsrc>
inline typename std::enable_if_t<sizeof(Tsrc) != sizeof(Tdest), Tdest> constexpr cast_if_required(Tsrc value) __attribute__((__const__)) __attribute__((__always_inline__));
template<typename Tdest, typename Tsrc>
inline typename std::enable_if_t<sizeof(Tsrc) == sizeof(Tdest), bool> constexpr is_same_size() __attribute__((__const__)) __attribute__((__always_inline__));
template<typename Tdest, typename Tsrc>
inline typename std::enable_if_t<sizeof(Tsrc) != sizeof(Tdest), bool> constexpr is_same_size() __attribute__((__const__)) __attribute__((__always_inline__));

/// Cast to a specified type if required - use in place of static_cast
template<typename Tdest, typename Tsrc> __attribute__((__always_inline__))
inline typename std::enable_if_t<sizeof(Tsrc) == sizeof(Tdest), Tdest> constexpr cast_if_required(Tsrc value) {
  return value;
}
template<typename Tdest, typename Tsrc> __attribute__((__always_inline__))
inline typename std::enable_if_t<sizeof(Tsrc) != sizeof(Tdest), Tdest> constexpr cast_if_required(Tsrc value) {
  return static_cast<Tdest>(value);
}

/// Determine if two types are the same size at compile time: is_same_size<unsigned int, size_t>()
template<typename Tdest, typename Tsrc> __attribute__((__always_inline__))
inline typename std::enable_if_t<sizeof(Tsrc) == sizeof(Tdest), bool> constexpr is_same_size() {
  return true;
}
template<typename Tdest, typename Tsrc> __attribute__((__always_inline__))
inline typename std::enable_if_t<sizeof(Tsrc) != sizeof(Tdest), bool> constexpr is_same_size() {
  return false;
}
