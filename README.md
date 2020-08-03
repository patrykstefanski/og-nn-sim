# og-nn-sim
An attempt to train the battle engine with neural networks.

## Requirements
* C++20 compiler (for dataset-gen; currently only Clang manages to compile it)
* CMake >= 3.12
* Python >= 3.7
* Tensorflow 2
* Pandas

## Instructions

### Building dataset generator
```shell script
cmake -S ./dataset-gen -B build -DCMAKE_BUILD_TYPE=Release -DDATASET_GEN_ENABLE_ARCH_NATIVE_OPT=On
cmake --build build
```

### Generating a dataset
This will generate a dataset and save it into _dataset_ file.
```shell script
./build/dataset-gen --dataset-size 1000000 --max-ships 10 --out dataset
```
Check also ```./build/dataset-gen --help```

### Training
Now, you can train the network on the generated dataset.
This will save the trained model info _model_ directory and the normalization scales into _scales_ file.
```shell script
./train.py
```

### Run battle example
```shell script
./battle.py
```

### Optimize model
Optionally, you can try to find a better number of layers or units in the model for your dataset:
```shell script
tensorboard --logdir logs &
./optimize-model.py
```
