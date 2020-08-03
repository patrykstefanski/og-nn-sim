#ifndef DATASET_GEN_UTIL_HPP
#define DATASET_GEN_UTIL_HPP

#include <cstdint>

namespace dataset_gen {

// Lehmer RNG
// rand() is too slow and there is no need for a good random.

constexpr std::uint64_t random_multiplier = 48271;
constexpr std::uint64_t random_modulus = 2147483647;
constexpr std::uint64_t random_max = random_modulus - 1;

constexpr std::uint32_t random_next(std::uint32_t r) {
  return static_cast<std::uint32_t>(static_cast<std::uint64_t>(r) * random_multiplier % random_modulus);
}

} // namespace dataset_gen

#endif // !DATASET_GEN_UTIL_HPP
