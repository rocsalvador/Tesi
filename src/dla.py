import numpy as np
import math
from textures import Textures
import tqdm

class DLA:
    def __init__(self, map_size, radius, aplha, sigma, tau, p):
        self.radius = radius
        self.generation_distance = 4 * radius + radius / 2
        self.death_radius = 2 * self.generation_distance
        self.map_size = map_size
        self.aggregate_distance = radius * 1.75
        self.move_distance = radius / 2

        # Lichens prob parameters
        self.alpha = aplha
        self.tau = tau
        self.sigma = sigma
        self.p = p

        # Sphere centers
        self.cluster = np.empty((0, 2))

        self.textures = Textures(self.map_size)

    def aggregation_prob(self, x, y):
        count = 0
        for aggregated_point in self.cluster:
            if self.euclidian_distance([x, y], aggregated_point) <= self.p * self.radius:
                count += 1
        return self.alpha + (1 - self.alpha) * pow(math.e, - self.sigma * (pow(count - self.tau, 2)))

    def euclidian_distance(self, p0, p1):
        distance = 0
        for x0, x1 in zip(p0, p1):
            distance += pow(x0 - x1, 2)
        return math.sqrt(distance)

    def get_random_point(self, center_idx):
        alpha = 2 * math.pi * np.random.uniform()
        x = self.generation_distance * math.cos(alpha) + self.cluster[center_idx][0]
        y = self.generation_distance * math.sin(alpha) + self.cluster[center_idx][1]

        while x < 0 or x >= self.map_size or y < 0 or y >= self.map_size:
            alpha = 2 * math.pi * np.random.uniform()
            x = self.generation_distance * math.cos(alpha) + self.cluster[center_idx][0]
            y = self.generation_distance * math.sin(alpha) + self.cluster[center_idx][1]

        return np.random.uniform(0, self.map_size), np.random.uniform(0, self.map_size)

    def aggregate(self, x, y, x_center, y_center):
        while True:
            p = self.aggregation_prob(x, y)
            for aggregated_point in self.cluster:
                distance = self.euclidian_distance([x, y], aggregated_point)
                if distance <= self.aggregate_distance:
                    if np.random.uniform() <= p:
                        point = np.array([[x, y]])
                        self.cluster = np.concatenate((self.cluster, point), axis=0)
                        return True
                    else:
                        return False

            # Move new point
            angle = np.random.uniform() * 2 * math.pi
            x_tmp = x + self.move_distance * math.cos(angle) 
            y_tmp = y + self.move_distance * math.sin(angle) 
            if self.euclidian_distance([x_tmp, y_tmp], [x_center, y_center]) > self.death_radius:
                return False
        
            while x_tmp < 0 or x_tmp >= self.map_size or y_tmp < 0 or y_tmp >= self.map_size:
                angle = np.random.uniform() * 2 * math.pi
                x_tmp = x + self.move_distance * math.cos(angle) 
                y_tmp = y + self.move_distance * math.sin(angle) 
                if self.euclidian_distance([x_tmp, y_tmp], [x_center, y_center]) > self.death_radius:
                    return False

            x = x_tmp
            y = y_tmp

    def get_center_points(self):
        return self.cluster
    
    def add_point(self):
        # Get a random point of the existing cluster
        rand_idx = np.random.randint(len(self.cluster))

        # Generate a random point at distance r
        x, y = self.get_random_point(rand_idx)
        # self.textures.add_point_to_gif(int(x), int(y))

        while not self.aggregate(x, y, self.cluster[rand_idx][0], self.cluster[rand_idx][1]):
            # self.textures.remove_point_from_gif(int(x), int(y))

            x, y = self.get_random_point(rand_idx)

            # self.textures.add_point_to_gif(int(x), int(y))

        self.textures.add_sphere(int(x), int(y), self.radius)

    def run(self, n_points):
        self.cluster = np.zeros((1, 2))
        center = int(self.map_size / 2)
        self.cluster[0] = [center, center]

        self.textures.add_point_to_gif(center, center)

        for _ in tqdm.tqdm(range(n_points)):
            self.add_point()
        
        self.textures.save("img_" + str(self.map_size) + "_" + str(self.radius) + "_" + str(self.alpha) + "_" + str(self.sigma) + "_" + str(self.tau) + "_" + str(self.p))  
