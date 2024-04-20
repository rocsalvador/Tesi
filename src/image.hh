#ifndef IMAGE_HH
#define IMAGE_HH

#include <vector>
#include <string>
using namespace std;

class Image 
{
public:
    Image();

    void save(const vector<vector<vector<uint8_t>>>& imageData, string filename);
};

#endif