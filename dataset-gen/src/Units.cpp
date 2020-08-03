#include "Units.hpp"

namespace dataset_gen {

const char *const unit_names[UnitKindEnd] = {
#define DATASET_GEN_DEF_UNIT_NAME(name) #name,
    DATASET_GEN_DEF_UNITS(DATASET_GEN_DEF_UNIT_NAME)
#undef DATASET_GEN_DEF_UNIT_NAME
};

// TODO: Add defense
const UnitAttrs unit_attrs[UnitKindEnd] = {
    [SmallCargo] =
        {
            .weapons = 5.0f,
            .shield = 10.0f,
            .armor = 4000.0f,
            .rapid_fire =
                {
                    [EspionageProbe] = 5,
                    [SolarSatellite] = 5,
                },
        },
    [LargeCargo] =
        {
            .weapons = 5.0f,
            .shield = 25.0f,
            .armor = 12000.0f,
            .rapid_fire =
                {
                    [EspionageProbe] = 5,
                    [SolarSatellite] = 5,
                },
        },
    [LightFighter] =
        {
            .weapons = 50.0f,
            .shield = 10.0f,
            .armor = 4000.0f,
            .rapid_fire =
                {
                    [EspionageProbe] = 5,
                    [SolarSatellite] = 5,
                },
        },
    [HeavyFighter] =
        {
            .weapons = 150.0f,
            .shield = 25.0f,
            .armor = 10000.0f,
            .rapid_fire =
                {
                    [SmallCargo] = 3,
                    [EspionageProbe] = 5,
                    [SolarSatellite] = 5,
                },
        },
    [Cruiser] =
        {
            .weapons = 400.0f,
            .shield = 50.0,
            .armor = 27000.0f,
            .rapid_fire =
                {
                    [LightFighter] = 6,
                    [EspionageProbe] = 5,
                    [SolarSatellite] = 5,
                    [RocketLauncher] = 10,
                },
        },
    [Battleship] =
        {
            .weapons = 1000.0f,
            .shield = 200.0f,
            .armor = 60000.0f,
            .rapid_fire =
                {
                    [EspionageProbe] = 5,
                    [SolarSatellite] = 5,
                },
        },
    [ColonyShip] =
        {
            .weapons = 50.0f,
            .shield = 100.0f,
            .armor = 30000.0f,
            .rapid_fire =
                {
                    [EspionageProbe] = 5,
                    [SolarSatellite] = 5,
                },
        },
    [Recycler] =
        {
            .weapons = 1.0f,
            .shield = 10.0f,
            .armor = 16000.0f,
            .rapid_fire =
                {
                    [EspionageProbe] = 5,
                    [SolarSatellite] = 5,
                },
        },
    [EspionageProbe] =
        {
            .weapons = 0.01f,
            .shield = 0.01f,
            .armor = 1000.0f,
            .rapid_fire = {},
        },
    [Bomber] =
        {
            .weapons = 1000.0f,
            .shield = 500.0f,
            .armor = 75000.0f,
            .rapid_fire =
                {
                    [EspionageProbe] = 5,
                    [SolarSatellite] = 5,
                    [RocketLauncher] = 20,
                    [LightLaser] = 20,
                    [HeavyLaser] = 10,
                    [GaussCannon] = 5,
                    [IonCannon] = 10,
                    [PlasmaTurret] = 5,
                },
        },
    [SolarSatellite] =
        {
            .weapons = 1.0f,
            .shield = 1.0f,
            .armor = 2000.0f,
            .rapid_fire = {},
        },
    [Destroyer] =
        {
            .weapons = 2000.0f,
            .shield = 500.0f,
            .armor = 110000.0f,
            .rapid_fire =
                {
                    [EspionageProbe] = 5,
                    [SolarSatellite] = 5,
                    [Battlecruiser] = 2,
                    [LightLaser] = 10,
                },
        },
    [DeathStar] =
        {
            .weapons = 200000.0f,
            .shield = 50000.0f,
            .armor = 9000000.0f,
            .rapid_fire =
                {
                    [SmallCargo] = 250,
                    [LargeCargo] = 250,
                    [LightFighter] = 200,
                    [HeavyFighter] = 100,
                    [Cruiser] = 33,
                    [Battleship] = 30,
                    [ColonyShip] = 250,
                    [Recycler] = 250,
                    [EspionageProbe] = 1250,
                    [Bomber] = 25,
                    [SolarSatellite] = 1250,
                    [Destroyer] = 5,
                    [Battlecruiser] = 15,
                    [RocketLauncher] = 200,
                    [LightLaser] = 200,
                    [HeavyLaser] = 100,
                    [GaussCannon] = 50,
                    [IonCannon] = 100,
                },
        },
    [Battlecruiser] =
        {
            .weapons = 700.0f,
            .shield = 400.0f,
            .armor = 70000.0f,
            .rapid_fire =
                {
                    [SmallCargo] = 3,
                    [LargeCargo] = 3,
                    [HeavyFighter] = 4,
                    [Cruiser] = 4,
                    [Battleship] = 7,
                    [EspionageProbe] = 5,
                    [SolarSatellite] = 5,
                },
        },
};

} // namespace dataset_gen
