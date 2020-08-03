#!/usr/bin/env python3

import pandas as pd
from tensorflow.keras.callbacks import TensorBoard

from train import INPUT_SIZE, create_model, load_data, normalize

NUM_LAYERS = range(2, 7)
NUM_UNITS = [32, 64, 128, 256, 512, 1024, 2048]


def optimize(df: pd.DataFrame):
    X, Y = df.iloc[:, :INPUT_SIZE], df.iloc[:, INPUT_SIZE:]

    for num_layers in NUM_LAYERS:
        for num_units in NUM_UNITS:
            name = '{}-{}'.format(num_layers, num_units)
            print(name)
            tb = TensorBoard(log_dir='logs/{}'.format(name))
            model = create_model(num_layers, num_units)
            model.fit(X, Y, epochs=20, validation_split=0.1, callbacks=[tb])


def main():
    df = load_data('dataset')
    _scales = normalize(df)
    optimize(df)


if __name__ == '__main__':
    main()
