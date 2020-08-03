#ifndef DATASET_GEN_UNITS_HPP
#define DATASET_GEN_UNITS_HPP

#include <cstdint>

namespace dataset_gen {

// Units

// clang-format off

#define DATASET_GEN_DEF_UNITS(_) \
  _(SmallCargo)                  \
  _(LargeCargo)                  \
  _(LightFighter)                \
  _(HeavyFighter)                \
  _(Cruiser)                     \
  _(Battleship)                  \
  _(ColonyShip)                  \
  _(Recycler)                    \
  _(EspionageProbe)              \
  _(Bomber)                      \
  _(SolarSatellite)              \
  _(Destroyer)                   \
  _(DeathStar)                   \
  _(Battlecruiser)               \
  _(RocketLauncher)              \
  _(LightLaser)                  \
  _(HeavyLaser)                  \
  _(GaussCannon)                 \
  _(IonCannon)                   \
  _(PlasmaTurret)                \
  _(SmallShieldDome)             \
  _(LargeShieldDome)

enum UnitKind : std::uint8_t {
#define DATASET_GEN_DEF_UNIT_KIND(name) name,
  DATASET_GEN_DEF_UNITS(DATASET_GEN_DEF_UNIT_KIND)
#undef DATASET_GEN_DEF_UNIT_KIND

  UnitKindEnd,
};

// clang-format on

extern const char *const unit_names[UnitKindEnd];

// Unit attributes

struct UnitAttrs {
  float weapons{};
  float shield{};
  float armor{};
  std::uint32_t rapid_fire[UnitKindEnd];
};

extern const UnitAttrs unit_attrs[UnitKindEnd];

} // namespace dataset_gen

#endif // !DATASET_GEN_UNITS_HPP
