import numpy as np
import os
from PIL import Image

class Textures:
    def __init__(self, n) -> None:
        self.n = n
        self.norm_map = np.zeros((n, n, 3), 'uint8')
        self.color_map = np.zeros((n, n, 3), 'uint8')
        self.roughness_map =  np.zeros((n, n), 'uint8')
        self.displacement_map =  np.zeros((n, n), 'uint8')

    def add_sphere(self, x, y, radius):
        color = [np.random.uniform(0, 128), np.random.uniform(192, 255), np.random.uniform(0, 128)]
        roughness = np.random.uniform(0, 255)
        
        for i in range(max(0, x - radius + 1), min(x + radius, self.n)):
            for j in range(max(0, y - radius + 1), min(y + radius, self.n)):
                distance = pow((pow((i - x), 2) + pow((j - y), 2)), 0.5)
                if distance <= radius:
                    norm_x = i - x
                    norm_y = j - y
                    norm_z = pow(pow(radius, 2) - pow(distance, 2), 0.5)
                    unit_factor = pow(pow(norm_x, 2) + pow(norm_y, 2) + pow(norm_z, 2), 0.5)
                    self.norm_map[i, j] = [127 + (norm_x / unit_factor * 128), 127 + (norm_y / unit_factor * 128), 127 + (norm_z / unit_factor * 128)]
                    
                    self.color_map[i, j] = color

                    self.roughness_map[i, j] = roughness

                    self.displacement_map[i, j] = norm_z / unit_factor * 255

    def save(self, dir="."):
        if not os.path.exists(dir):
            os.mkdir(dir)
        
        norm = Image.fromarray(self.norm_map, "RGB")
        norm.save(os.path.join(dir, "norm_map.png"))

        color = Image.fromarray(self.color_map, "RGB")
        color.save(os.path.join(dir, "color_map.png"))

        roughness = Image.fromarray(self.roughness_map, "L")
        roughness.save(os.path.join(dir, "roughness_map.png"))

        displacement = Image.fromarray(self.displacement_map, "L")
        displacement.save(os.path.join(dir, "displacement_map.png"))