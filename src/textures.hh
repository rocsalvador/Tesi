#ifndef TEXTURES_HH
#define TEXTURES_HH

#include "image.hh"
#include "point.hh"

#include <vector>
#include <filesystem>
#include <fstream>
using namespace std;

class Textures
{
private:
    int height, width;

    vector<vector<vector<uint8_t>>> colorMap;

    vector<Point<int>> drawnPoints;

    vector<vector<bool>> drawnMap;

public:
    Textures(int width, int height);

    void readSphereCenterPoints(string filename, vector<uint8_t> color, int radius);

    void drawPoints(const vector<Point<int>>& points, vector<uint8_t> color);

    void drawCircle(const Point<int>& point, vector<uint8_t> color, int radius);

    void drawCircles(const vector<Point<int>>& points, vector<uint8_t> color, int radius);

    const vector<Point<int>>& getDrawnPoints();

    void save(string dirname = ".", string name = "");

    void clear();
};

#endif