#ifndef IMAGE_HH
#define IMAGE_HH

#include <vector>
#include <string>
using namespace std;

class Image 
{
private:
    uint8_t **imageDataC;
public:
    Image();

    vector<vector<vector<uint8_t>>> load(string filename);

    void save(const vector<vector<vector<uint8_t>>>& imageData, string filename);
};

#endif