#include "BattleEngine.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <utility>
#include <vector>

#include "Units.hpp"
#include "Util.hpp"

namespace dataset_gen {

namespace {

Party create_party(std::vector<Combatant> &combatants) {
  assert(combatants.size() <= std::numeric_limits<std::uint8_t>::max());

  std::uint32_t total_units = 0;
  for (const auto &combatant : combatants)
    total_units += std::accumulate(combatant.unit_groups.cbegin(), combatant.unit_groups.cend(), std::uint32_t{0});

  std::vector<Unit> units{};
  units.reserve(total_units);
  for (std::size_t i = 0; i < combatants.size(); ++i) {
    const auto &combatant = combatants[i];
    for (std::uint8_t kind = 0; kind < combatant.unit_groups.size(); ++kind) {
      float max_hull = 0.1f * unit_attrs[kind].armor * (1.0f + 0.1f * combatant.techs.armor);
      for (std::uint32_t j = 0; j < combatant.unit_groups[kind]; ++j)
        units.push_back(Unit{
            .shield = 0.0f,
            .hull = max_hull,
            .kind = kind,
            .combatant_id = static_cast<std::uint8_t>(i),
        });
    }
  }

  return {
      .combatants = combatants,
      .units = std::move(units),
      .num_alive = total_units,
  };
}

void restore_shields(Party &party) {
  for (std::uint32_t i = 0; i < party.num_alive; ++i) {
    Unit &unit = party.units[i];
    unit.shield = unit_attrs[unit.kind].shield * (1.0f + 0.1f * party.combatants[unit.combatant_id].techs.shielding);
  }
}

void fire(const Party &attackers_party, Party &defenders_party, std::uint32_t &random) {
  std::uint32_t r = random;

  const Combatant *attackers = attackers_party.combatants.data();
  const Unit *shooters = attackers_party.units.data();
  std::uint32_t num_shooters = attackers_party.num_alive;

  const Combatant *defenders = defenders_party.combatants.data();
  Unit *targets = defenders_party.units.data();
  std::uint32_t num_targets = defenders_party.num_alive;

  for (std::uint32_t i = 0; i < num_shooters; ++i) {
    const Unit &shooter = shooters[i];
    const UnitAttrs &shooter_attrs = unit_attrs[shooter.kind];

    const float damage = shooter_attrs.weapons * (1.0f + 0.1f * attackers[shooter.combatant_id].techs.weapons);
    std::uint32_t rapid_fire;

    do {
      r = random_next(r);
      Unit &target = targets[r % num_targets];
      const UnitAttrs &target_attrs = unit_attrs[target.kind];
      const Combatant &defender = defenders[target.combatant_id];

      if (target.hull != 0.0f) {
        float hull = target.hull;
        float hull_damage = damage - target.shield;

        if (hull_damage < 0.0f) {
          const float max_shield = target_attrs.shield * (1.0f + 0.1f * defender.techs.shielding);
          float shield_damage = 0.01f * std::floor(100.0f * damage / max_shield) * max_shield;
          target.shield -= shield_damage;
        } else {
          target.shield = 0.0f;
          if (hull_damage > hull)
            hull_damage = hull;
          hull -= hull_damage;
        }

        if (hull != 0.0f) {
          const float max_hull = 0.1f * target_attrs.armor * (1.0f + 0.1f * defender.techs.armor);
          if (hull < 0.7f * max_hull) {
            r = random_next(r);
            if (hull < (1.0f / static_cast<float>(random_max)) * static_cast<float>(r) * max_hull)
              hull = 0.0f;
          }
        }
        target.hull = hull;
      }

      rapid_fire = shooter_attrs.rapid_fire[target.kind];
    } while (rapid_fire != 0 && (r = random_next(r)) % rapid_fire != 0);
  }

  random = r;
}

void update_units(Party &party) {
  Unit *units = party.units.data();
  std::uint32_t n = 0;
  for (std::uint32_t i = 0; i < party.num_alive; ++i) {
    if (units[i].hull != 0.0f)
      units[n++] = units[i];
  }
  party.num_alive = n;
}

void update_combatants(Party &party) {
  for (auto &combatant : party.combatants)
    std::fill(combatant.unit_groups.begin(), combatant.unit_groups.end(), 0);

  for (std::uint32_t i = 0; i < party.num_alive; ++i) {
    const Unit &unit = party.units[i];
    ++party.combatants[unit.combatant_id].unit_groups[unit.kind];
  }
}

} // namespace

std::uint32_t fight(std::vector<Combatant> &attackers, std::vector<Combatant> &defenders, std::uint32_t seed) {
  Party attackers_party = create_party(attackers);
  Party defenders_party = create_party(defenders);

  constexpr std::uint32_t max_rounds = 6;
  std::uint32_t round = 0;

  while (round < max_rounds && attackers_party.num_alive > 0 && defenders_party.num_alive > 0) {
    restore_shields(attackers_party);
    restore_shields(defenders_party);

    fire(attackers_party, defenders_party, seed);
    fire(defenders_party, attackers_party, seed);

    update_units(attackers_party);
    update_units(defenders_party);

    ++round;
  }

  update_combatants(attackers_party);
  update_combatants(defenders_party);

  return round;
}

} // namespace dataset_gen
