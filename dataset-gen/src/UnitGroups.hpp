#ifndef DATASET_GEN_UNIT_GROUPS_HPP
#define DATASET_GEN_UNIT_GROUPS_HPP

#include <array>
#include <cmath>
#include <cstdint>

#include "Units.hpp"

namespace dataset_gen {

template <typename T> using UnitGroups = std::array<T, UnitKindEnd>;

template <typename To, typename From> UnitGroups<To> convert(const UnitGroups<From> &from) {
  UnitGroups<To> ret;
  for (std::uint32_t kind = 0; kind < UnitKindEnd; ++kind)
    ret[kind] = static_cast<To>(from[kind]);
  return ret;
}

template <typename T> UnitGroups<T> operator+(const UnitGroups<T> &lhs, const UnitGroups<T> &rhs) {
  UnitGroups<T> ret;
  for (std::uint32_t kind = 0; kind < UnitKindEnd; ++kind)
    ret[kind] = lhs[kind] + rhs[kind];
  return ret;
}

template <typename T> UnitGroups<T> operator-(const UnitGroups<T> &lhs, const UnitGroups<T> &rhs) {
  UnitGroups<T> ret;
  for (std::uint32_t kind = 0; kind < UnitKindEnd; ++kind)
    ret[kind] = lhs[kind] - rhs[kind];
  return ret;
}

template <typename T> UnitGroups<T> operator*(const UnitGroups<T> &lhs, const UnitGroups<T> &rhs) {
  UnitGroups<T> ret;
  for (std::uint32_t kind = 0; kind < UnitKindEnd; ++kind)
    ret[kind] = lhs[kind] * rhs[kind];
  return ret;
}

template <typename T> UnitGroups<T> operator/(const UnitGroups<T> &groups, T scalar) {
  UnitGroups<T> ret;
  for (std::uint32_t kind = 0; kind < UnitKindEnd; ++kind)
    ret[kind] = groups[kind] / scalar;
  return ret;
}

template <typename T> UnitGroups<T> sq_root(const UnitGroups<T> &groups) {
  UnitGroups<T> ret;
  for (std::uint32_t kind = 0; kind < UnitKindEnd; ++kind)
    ret[kind] = std::sqrt(groups[kind]);
  return ret;
}

} // namespace dataset_gen

#endif // !DATASET_GEN_UNIT_GROUPS_HPP
