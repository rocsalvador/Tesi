#ifndef IMAGE_HH
#define IMAGE_HH

#include <vector>
#include <string>
#include <filesystem>
using namespace std;

class Image 
{
private:
    uint8_t **imageDataC;
public:
    Image();

    vector<vector<vector<uint8_t>>> load(string filename, uint components = 4);

    void save(const vector<vector<vector<uint8_t>>>& imageData, string filename, uint components = 4);
};

#endif