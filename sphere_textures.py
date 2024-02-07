import numpy as np
import random
from PIL import Image

n = 1024
r = 40
threshold_prob = 0.0001

norm_map = np.zeros((n, n, 3), 'uint8')
color_map = np.zeros((n, n, 3), 'uint8')
roughness_map =  np.zeros((n, n), 'uint8')

def draw_sphere_textures(x, y, radius):
    color = [random.uniform(0, 128), random.uniform(192, 255), random.uniform(0, 128)]
    roughness = random.uniform(0, 255)
    for i in range(x - radius, x + radius):
        for j in range(y - radius, y + radius):
            if (i >= 0 and i < n and j >= 0 and j < n and pow((pow((i - x), 2) + pow((j - y), 2)), 0.5) <= radius):
                norm_x = abs(i - x)
                norm_y = abs(j - y)
                xy_length = pow(pow(norm_x, 2) + pow(norm_y, 2), 0.5)
                norm_z = pow(pow(radius, 2) - pow(xy_length, 2), 0.5)
                unit_factor =  (norm_x + norm_y + norm_z)
                norm_map[i, j] = [norm_x / unit_factor * 255, norm_y / unit_factor * 255, norm_z / unit_factor * 255]

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