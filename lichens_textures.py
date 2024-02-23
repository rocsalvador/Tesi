from dla import DLA
from textures import Textures

if __name__ == "__main__":
    dla = DLA(128, 10)
    dla.run()

    textures = Textures(128)
    for center_point in dla.get_center_points():
        textures.add_sphere(int(center_point[0]), int(center_point[1]), 1)
    
    textures.save()