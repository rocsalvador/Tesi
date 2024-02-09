import numpy as np
import random
from PIL import Image
import sys

if len(sys.argv) < 4:
    print("Usage:")
    print("python3 sphere_textures.py n r p")
    exit(1)

n = int(sys.argv[1])
r = int(sys.argv[2])
threshold_prob = float(sys.argv[3])

norm_map = np.zeros((n, n, 3), 'uint8')
color_map = np.zeros((n, n, 3), 'uint8')
roughness_map =  np.zeros((n, n), 'uint8')

def draw_sphere_textures(x, y, radius):
    color = [random.uniform(0, 128), random.uniform(192, 255), random.uniform(0, 128)]
    roughness = random.uniform(0, 255)
    
    for i in range(max(0, x - radius + 1), min(x + radius, n)):
        for j in range(max(0, y - radius + 1), min(y + radius, n)):
            distance = pow((pow((i - x), 2) + pow((j - y), 2)), 0.5)
            if distance <= radius:
                norm_x = i - x
                norm_y = j - y
                norm_z = pow(pow(radius, 2) - pow(distance, 2), 0.5)
                unit_factor =  (abs(norm_x) + abs(norm_y) + norm_z)
                norm_map[i, j] = [127 + (norm_x / unit_factor * 128), 127 + (norm_y / unit_factor * 128), 127 + (norm_z / unit_factor * 128)]
                
                color_map[i, j] = color

                roughness_map[i, j] = roughness


for i in range(0, n):
    for j in range(0, n):
        rand = random.uniform(0, 1)
        if rand <= threshold_prob:
            draw_sphere_textures(i, j, r)

norm = Image.fromarray(norm_map, "RGB")
norm.save("norm_map.png")

color = Image.fromarray(color_map, "RGB")
color.save("color_map.png")

roughness = Image.fromarray(roughness_map, "L")
roughness.save("roughness_map.png")