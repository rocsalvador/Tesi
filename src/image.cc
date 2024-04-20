#include "image.hh"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include <iostream>

Image::Image() {}

void Image::save(const vector<vector<vector<uint8_t>>>& imageData, string filename)
{
    uint height = imageData.size();
    assert(height > 0);
    uint width = imageData[0].size();
    uint8_t imageDataC[width * height * 3];
    for (uint i = 0; i < height; ++i)
    {
        for (uint j = 0; j < width; ++j)
        {
            for (uint k = 0; k < 3; ++k)
                imageDataC[3 * (i * width + j) + k] = imageData[i][j][k];
        }
    }
    stbi_write_png(filename.c_str(), width, height, 3, imageDataC, width * 3);
}