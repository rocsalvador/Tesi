import numpy as np
import math
from PIL import Image
import os

class DLA:
    def __init__(self, n, r):
        self.sphere_r = 10
        self.t = 1
        self.r = r
        self.n = n
        self.timestep= 0

        # Lichens prob parameters
        self.alpha = 0.001
        self.tau = 3
        self.theta = 1.5

        # Sphere centers
        self.cluster = np.empty((0, 2))
        # Map
        self.map = np.zeros((self.n, self.n), 'uint8')

        if not os.path.exists("out"):
            os.mkdir("out")

    def aggregation_prob(self, x, y):
        count = 0
        for i in range(int(max(0, x - 3)), int(min(x + 4, self.n))):
            for j in range(int(max(0, y - 3)), int(min(y + 4, self.n))):
                if self.map[i][j]:
                    count += 1
        return self.alpha + (1 - self.alpha) * pow(math.e, - self.theta * (pow(count - self.tau, 2)))

    def print_map(self):
        img = Image.fromarray(self.map * 255, "L")
        img.save("out/" + str(self.timestep) + ".png")
        self.timestep+= 1

    def euclidian_distance(self, p0, p1):
        distance = 0
        for x0, x1 in zip(p0, p1):
            distance += pow(x0 - x1, 2)
        return math.sqrt(distance)
    
    def get_random_point(self, center_idx):
        alpha = 2 * math.pi * np.random.random()
        x = int(self.r * math.cos(alpha) + self.cluster[center_idx][0])
        y = int(self.r * math.sin(alpha) + self.cluster[center_idx][1])

        while x < 0 or x >= self.n or y < 0 or y >= self.n or self.map[x][y]:
            alpha = 2 * math.pi * np.random.random()
            x = int(self.r * math.cos(alpha) + self.cluster[center_idx][0])
            y = int(self.r * math.sin(alpha) + self.cluster[center_idx][1])
        
        return x, y

    def aggregate(self, x, y, x_center, y_center):
        while True:
            for i in range(int(max(0, x - 1)), int(min(x + 2, self.n))):
                for j in range(int(max(0, y - 1)), int(min(y + 2, self.n))):
                    if i != x and j != y and self.map[i][j]:
                        p = self.aggregation_prob(x, y)
                        if np.random.uniform() <= p:
                            self.map[x][y] = 1
                            point = np.array([[x, y]])
                            self.cluster = np.concatenate((self.cluster, point), axis=0)
                            return True
            
            # Move new point
            x_tmp = x + 1 - np.random.randint(2)
            y_tmp = y + 1 - np.random.randint(2)
            if self.euclidian_distance([x_tmp, y_tmp], [x_center, y_center]) > self.r:
                return False
            
            while x_tmp < 0 or x_tmp >= self.n or y_tmp < 0 or y_tmp >= self.n or self.map[x_tmp, y_tmp]:
                x_tmp = x + 1 - np.random.randint(2)
                y_tmp = y + 1 - np.random.randint(2)
                if self.euclidian_distance([x_tmp, y_tmp], [x_center, y_center]) > self.r:
                    return False
            
            x = x_tmp
            y = y_tmp

    def get_center_points(self):
        return self.cluster

    def run(self):
        self.cluster = np.zeros((1, 2))
        center = int(self.n / 2)
        self.cluster[0] = [center, center]
        self.map [center, center] = 1
        i = 0
        while i < 300:
            # Get a random point of the existing cluster
            rand_idx = np.random.randint(len(self.cluster))

            # Generate a random point at distance r
            x, y = self.get_random_point(rand_idx)

            while not self.aggregate(x, y, self.cluster[rand_idx][0], self.cluster[rand_idx][1]):
                x, y = self.get_random_point(rand_idx)
            
            i += 1
            print(i)

        self.print_map()