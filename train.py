#!/usr/bin/env python3

import pandas as pd
from tensorflow.keras import Model, Sequential
from tensorflow.keras.layers import Dense

NUM_TECHS = 3
NUM_UNIT_KINDS = 14

# Input should have technologies and units vector for each of the two combatants
# Output should have mean and sd values per each unit kind for each of the combatants
INPUT_SIZE = 2 * (NUM_TECHS + NUM_UNIT_KINDS)
OUTPUT_SIZE = 2 * (2 * NUM_UNIT_KINDS)


def load_data(dataset_path: str) -> pd.DataFrame:
    df = pd.read_csv(dataset_path, header=None)
    _, num_cols = df.shape
    assert num_cols == INPUT_SIZE + OUTPUT_SIZE
    return df


def normalize(df: pd.DataFrame) -> (float, float, float, float):
    max_tech = df.iloc[:, 0:(NUM_TECHS * 2)].max().max()
    max_ships = df.iloc[:, (NUM_TECHS * 2):INPUT_SIZE].max().max()
    max_mean = df.iloc[:, INPUT_SIZE:(INPUT_SIZE + 2 * NUM_UNIT_KINDS)].max().max()
    max_sd = df.iloc[:, (INPUT_SIZE + 2 * NUM_UNIT_KINDS):(INPUT_SIZE + OUTPUT_SIZE)].max().max()

    tech_scale = 1.0 / max_tech
    ships_scale = 1.0 / max_ships
    mean_scale = 1.0 / max_mean
    sd_scale = 1.0 / max_sd

    df.iloc[:, 0:(NUM_TECHS * 2)] *= tech_scale
    df.iloc[:, (NUM_TECHS * 2):INPUT_SIZE] *= ships_scale
    df.iloc[:, INPUT_SIZE:(INPUT_SIZE + 2 * NUM_UNIT_KINDS)] *= mean_scale
    df.iloc[:, (INPUT_SIZE + 2 * NUM_UNIT_KINDS):(INPUT_SIZE + OUTPUT_SIZE)] *= sd_scale

    return tech_scale, ships_scale, mean_scale, sd_scale


def create_model(num_layers: int, num_units: int) -> Sequential:
    assert num_layers >= 1
    assert num_units >= 1
    model = Sequential()
    for i in range(num_layers - 1):
        model.add(Dense(num_units, activation='relu'))
    model.add(Dense(OUTPUT_SIZE, activation='relu'))
    model.compile(optimizer='adam', loss='mse', metrics=['mse'])
    return model


def train(df: pd.DataFrame, model: Model, num_epochs: int):
    X, Y = df.iloc[:, :INPUT_SIZE], df.iloc[:, INPUT_SIZE:]
    model.fit(X, Y, epochs=num_epochs, validation_split=0.1)


def main():
    df = load_data('dataset')
    scales = normalize(df)
    model = create_model(num_layers=4, num_units=1024)
    train(df, model, num_epochs=20)
    model.save('model')
    with open('scales', 'w') as f:
        f.write(' '.join(map(str, scales)))


if __name__ == '__main__':
    main()
