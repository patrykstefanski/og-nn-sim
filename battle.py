#!/usr/bin/env python3

from typing import List

from tensorflow.keras.models import load_model

UNITS = [
    'SmallCargo',
    'LargeCargo',
    'LightFighter',
    'HeavyFighter',
    'Cruiser',
    'Battleship',
    'ColonyShip',
    'Recycler',
    'EspionageProbe',
    'Bomber',
    'SolarSatellite',
    'Destroyer',
    'DeathStar',
    'Battlecruiser',
]
NUM_UNITS = len(UNITS)

tech_scale = 0
ships_scale = 0
mean_scale = 0
sd_scale = 0


def load_scales(path: str):
    global tech_scale, ships_scale, mean_scale, sd_scale
    with open(path) as f:
        tech_scale, ships_scale, mean_scale, sd_scale = map(float, f.read().strip().split())


def make_input(attacker, defender) -> (List[float], float):
    ret = []
    for combatant in [attacker, defender]:
        for tech in ['weapons', 'shielding', 'armor']:
            ret.append(combatant[tech] * tech_scale)
    max_ships = max(n for c in [attacker, defender] for n in c['units'].values())
    scale = 1.0
    if max_ships > 1.0 / ships_scale:
        scale = (1.0 / ships_scale) / max_ships
    for combatant in [attacker, defender]:
        for kind in UNITS:
            try:
                num_ships = combatant['units'][kind]
            except KeyError:
                num_ships = 0
            ret.append(num_ships * ships_scale * scale)
    return ret, scale


def dump_results(results: List[float], scale: float):
    for i in range(2):
        for j, kind in enumerate(UNITS):
            mean = results[i * NUM_UNITS + j]
            mean_scaled = mean / (mean_scale * scale)
            sd = results[i * NUM_UNITS + j + NUM_UNITS]
            sd_scaled = sd / (sd_scale * scale)
            if round(mean_scaled, 1) < 0.1:
                continue
            print('{:15}\t{:.2f} ± {:.4f}'.format(kind, mean_scaled, sd_scaled))
        print()


def main():
    attacker = {
        'weapons': 10,
        'shielding': 10,
        'armor': 10,
        'units': {
            'HeavyFighter': 50,
            'Cruiser': 10,
            'Battleship': 5,
        }
    }
    defender = {
        'weapons': 10,
        'shielding': 10,
        'armor': 10,
        'units': {
            'LightFighter': 30,
            'Battleship': 15,
        }
    }

    load_scales('scales')
    model = load_model('model')
    inputs, scale = make_input(attacker, defender)
    prediction = model.predict([inputs])[0]
    dump_results(prediction, scale)


if __name__ == '__main__':
    main()
