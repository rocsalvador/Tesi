import numpy as np
import math
from PIL import Image
import os

class DLA:
    def __init__(self, n, r):
        self.t = 1
        self.r = r
        self.n = n
        self.aggregate_distance = 20
        self.timestep= 0

        # Lichens prob parameters
        self.alpha = 0.001
        self.tau = 3
        self.theta = 1.5

        # Sphere centers
        self.cluster = np.empty((0, 2))

    def aggregation_prob(self, x, y):
        count = 0
        for aggregated_point in self.cluster:
            if self.euclidian_distance([x, y], aggregated_point) <= 10:
                count += 1
        return self.alpha + (1 - self.alpha) * pow(math.e, - self.theta * (pow(count - self.tau, 2)))

    def euclidian_distance(self, p0, p1):
        distance = 0
        for x0, x1 in zip(p0, p1):
            distance += pow(x0 - x1, 2)
        return math.sqrt(distance)
    
    def get_random_point(self, center_idx):
        alpha = 2 * math.pi * np.random.random()
        x = self.r * math.cos(alpha) + self.cluster[center_idx][0]
        y = self.r * math.sin(alpha) + self.cluster[center_idx][1]

        while x < 0 or x >= self.n or y < 0 or y >= self.n:
            alpha = 2 * math.pi * np.random.random()
            x = self.r * math.cos(alpha) + self.cluster[center_idx][0]
            y = self.r * math.sin(alpha) + self.cluster[center_idx][1]
        
        return x, y

    def aggregate(self, x, y, x_center, y_center):
        while True:
            p = self.aggregation_prob(x, y)
            for aggregated_point in self.cluster:
                distance = self.euclidian_distance([x, y], aggregated_point)
                if distance >= 3 and distance <= 4:
                    if np.random.uniform() <= p:
                        point = np.array([[x, y]])
                        self.cluster = np.concatenate((self.cluster, point), axis=0)
                        return True
    
            # Move new point
            x_tmp = x + 1 - np.random.uniform() * 2
            y_tmp = y + 1 - np.random.uniform() * 2
            if self.euclidian_distance([x_tmp, y_tmp], [x_center, y_center]) > self.r:
                return False
            
            while x_tmp < 0 or x_tmp >= self.n or y_tmp < 0 or y_tmp >= self.n:
                x_tmp = x + 1 - np.random.uniform() * 2
                y_tmp = y + 1 - np.random.uniform() * 2
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

        i = 0
        while i < 1000:
            # Get a random point of the existing cluster
            rand_idx = np.random.randint(len(self.cluster))

            # Generate a random point at distance r
            x, y = self.get_random_point(rand_idx)

            while not self.aggregate(x, y, self.cluster[rand_idx][0], self.cluster[rand_idx][1]):
                x, y = self.get_random_point(rand_idx)
            
            i += 1
            print(i)