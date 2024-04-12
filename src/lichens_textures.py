import sys
import os
from textures import Textures

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage:")
        print("python3 lichens_textures.py img_size points_filename")
        exit(1)
 
    img_size = int(sys.argv[1])
    points_filename = sys.argv[2]
    basename = os.path.splitext(os.path.basename(points_filename))[0]

    textures = Textures(img_size)
    textures.read_center_points(points_filename)
    textures.save("img_" + basename)