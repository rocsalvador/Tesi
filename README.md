# Simulation and visualization of lichens growth in cultural heritage 3D models

I use [stb](https://github.com/nothings/stb/tree/master) single-file public domain libraries for C/C++ to read/write images.

## How to run

```bash
# Get the repo
git clone --recurse-submodules  https://github.com/rocsalvador/Tesi.git
cd Tesi
```

## Compile

```bash
make -j
```

## Run

```bash
# Make stack size unlimited because of how images are saved for now
ulimit -s unlimited
./lichens config.conf
```

## Configure

To configure the DLA, KMeans and parametrization modify the contents of the config.conf file:
```
img_dir: img

#DLA_config
# centers_file: examples/large_lichen_centers.txt
centers_file: -
points: 2000
alpha: 0.01
tau: 5
sigma: 0.3
p: 2
radius: 6

#KMeans_config
max_it: 40
k: 40
min_cluster_size: 400
min_circularity: 0.9

#Parametrization config
texture_name_prefix: img/param/original_lichen_texture/lichen_
scale_ratio: 5
```
