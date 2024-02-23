from textures import Textures
import numpy as np
import sys

if len(sys.argv) < 4:
    print("Usage:")
    print("python3 sphere_textures.py n r p")
    exit(1)

n = int(sys.argv[1])
r = int(sys.argv[2])
threshold_prob = float(sys.argv[3])

textures = Textures(n)

for i in range(0, n):
    for j in range(0, n):
        rand = np.random.uniform(0, 1)
        if rand <= threshold_prob:
            textures.add_sphere(i, j, r)

textures.save("img")