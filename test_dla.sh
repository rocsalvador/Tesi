#!/bin/bash

n_points=2000
map_size=(1024 1024 1024 1024 1024 1024)
radius=(5 5 5 5 5 5)
sigma=(3 3 0.3 0.3 0.3 0.3)
alpha=(0.0001 0.0001 0.001 0.01 0.0001 0.01)
tau=(1.5 3 2 5 4 1.5)
p=(2.5 2.5 2.5 2 2.5 2.25)

make

i=0
while [ $i -lt ${#map_size[@]} ]; do
    ./dla $n_points ${map_size[i]} ${radius[i]} ${alpha[i]} ${sigma[i]} ${tau[i]} ${p[i]} && 
    python3 src/lichens_textures.py ${map_size[i]} lichens_m_${map_size[i]}_r_${radius[i]}_a_${alpha[i]}_s_${sigma[i]}_t_${tau[i]}_p_${p[i]}.txt &&
    echo "Run test with map size ${map_size[i]}, radius ${radius[i]}, alpha ${alpha[i]}, sigma ${sigma[i]}, tau ${tau[i]}, p ${p[i]}" &
    i=$((i+1))
done

