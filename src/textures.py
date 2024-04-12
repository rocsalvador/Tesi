import numpy as np
import os
from PIL import Image
import random


class Textures:
    def __init__(self, n) -> None:
        self.n = n
        self.norm_map = np.zeros((n, n, 3), 'uint8')
        self.color_map = np.zeros((n, n, 3), 'uint8')
        self.roughness_map = np.zeros((n, n), 'uint8')
        self.displacement_map = np.zeros((n, n), 'uint8')

        self.center_points = []

        self.gif_frames = []
        self.gif_map = np.zeros((n, n), 'uint8')
    
    def add_point_to_gif(self, x, y):
        self.gif_map[x, y] = 255
        self.add_frame_to_gif()

    def remove_point_from_gif(self, x, y):
        self.gif_map[x, y] = 0
        self.add_frame_to_gif()

    def add_frame_to_gif(self):
        frame = Image.fromarray(self.gif_map.copy(), "L")
        self.gif_frames.append(frame)

    def add_sphere(self, x, y, radius):
        color = [np.random.uniform(0, 128),
                 np.random.uniform(192, 255),
                 np.random.uniform(0, 128)]
        roughness = np.random.uniform(0, 255)

        for j in range(min(self.n - 1, y + radius), max(y - radius - 1, -1), -1):
            i = x
            distance = pow((pow((i - x), 2) + pow((j - y), 2)), 0.5)
            while distance <= radius:
                if i < self.n:
                    norm_x = i - x
                    norm_y = j - y
                    norm_z = pow(pow(radius, 2) - pow(distance, 2), 0.5)
                    unit_factor = pow(pow(norm_x, 2) + pow(norm_y, 2) + pow(norm_z, 2), 0.5)
                    self.norm_map[i, j] = [127.5 + (norm_x / unit_factor * 127.5),
                                            127.5 + (norm_y / unit_factor * 127.5),
                                            127.5 + (norm_z / unit_factor * 127.5)]

                    self.color_map[i, j] = color

                    self.roughness_map[i, j] = roughness

                    self.displacement_map[i, j] = norm_z / unit_factor * 255

                i_inv = i - 2 * (i - x) 
                if i_inv >= 0:
                    norm_x = i_inv - x
                    norm_y = j - y
                    norm_z = pow(pow(radius, 2) - pow(distance, 2), 0.5)
                    unit_factor = pow(pow(norm_x, 2) + pow(norm_y, 2) + pow(norm_z, 2), 0.5)
                    self.norm_map[i_inv, j] = [127.5 + (norm_x / unit_factor * 127.5),
                                            127.5 + (norm_y / unit_factor * 127.5),
                                            127.5 + (norm_z / unit_factor * 127.5)]

                    self.color_map[i_inv, j] = color

                    self.roughness_map[i_inv, j] = roughness

                    self.displacement_map[i_inv, j] = norm_z / unit_factor * 255
                
                i += 1
                distance = pow((pow((i - x), 2) + pow((j - y), 2)), 0.5)


    def read_center_points(self, filename):
        with open(filename, 'r') as fobj:
            for i, line in enumerate(fobj):
                if i == 0:
                    radius = int(line)
                else:
                    xy = [float(num) for num in line.split()]
                    self.add_sphere(int(xy[0]), int(xy[1]), radius)

    def draw_center_points(self, radius):
        for xy in self.center_points:
            self.add_sphere(int(xy[0]), int(xy[1]), radius)

    def save(self, dir="."):
        os.makedirs(dir, exist_ok=True)

        norm = Image.fromarray(self.norm_map, "RGB")
        norm.save(os.path.join(dir, "norm_map.png"))

        color = Image.fromarray(self.color_map, "RGB")
        color.save(os.path.join(dir, "color_map.png"))

        roughness = Image.fromarray(self.roughness_map, "L")
        roughness.save(os.path.join(dir, "roughness_map.png"))

        displacement = Image.fromarray(self.displacement_map, "L")
        displacement.save(os.path.join(dir, "displacement_map.png"))

        if len(self.gif_frames) > 1:
            self.gif_frames[0].save(os.path.join(dir, "animation.gif"),
                                    save_all=True, append_images=self.gif_frames[1:], loop=0, duration=10)
