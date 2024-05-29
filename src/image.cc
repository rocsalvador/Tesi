#include "image.hh"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include <iostream>

Image::Image() {}

vector<vector<vector<uint8_t>>> Image::load(string filename, uint components)
{
    if (not filesystem::exists(filename))
    {
        cerr << "[ERROR] Can not open image file " << filename << endl;
        return vector<vector<vector<uint8_t>>> (0, vector<vector<uint8_t>> (0, vector<uint8_t> (0)));
    }

    int height;
    int width;
    int comp;
    unsigned char* imageC = stbi_load(filename.c_str(), &width, &height, &comp, components);
    vector<vector<vector<uint8_t>>> image (width, vector<vector<uint8_t>> (height, vector<uint8_t> (4)));
    for (uint i = 0; i < height; ++i)
        for (uint j = 0; j < width; ++j)
            for (uint k = 0; k < components; ++k)
                image[i][j][k] = imageC[components * (i * width + j) + k];
    stbi_image_free(imageC);
    return image;
}

void Image::save(const vector<vector<vector<uint8_t>>>& imageData, string filename, uint components)
{
    uint height = imageData.size();
    assert(height > 0);
    uint width = imageData[0].size();
    uint8_t imageDataC[width * height * components];
    for (uint i = 0; i < height; ++i)
        for (uint j = 0; j < width; ++j)
            for (uint k = 0; k < components; ++k)
                imageDataC[components * (i * width + j) + k] = imageData[i][j][k];
    stbi_write_png(filename.c_str(), width, height, components, imageDataC, width * components);
}