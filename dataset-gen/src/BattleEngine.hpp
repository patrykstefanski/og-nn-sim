#ifndef DATASET_GEN_BATTLE_ENGINE_HPP
#define DATASET_GEN_BATTLE_ENGINE_HPP

#include <cstdint>
#include <vector>

#include "UnitGroups.hpp"

namespace dataset_gen {

struct CombatTechs {
  std::uint8_t weapons{};
  std::uint8_t shielding{};
  std::uint8_t armor{};
};

struct Combatant {
  CombatTechs techs{};
  UnitGroups<std::uint32_t> unit_groups{};
};

struct Unit {
  float shield{};
  float hull{};
  std::uint8_t kind{};
  std::uint8_t combatant_id{};
};

struct Party {
  std::vector<Combatant> &combatants;
  std::vector<Unit> units{};
  std::uint32_t num_alive{};
};

std::uint32_t fight(std::vector<Combatant> &attackers, std::vector<Combatant> &defenders, std::uint32_t seed);

} // namespace dataset_gen

#endif // !DATASET_GEN_BATTLE_ENGINE_HPP
