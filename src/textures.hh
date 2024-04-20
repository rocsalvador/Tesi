#ifndef TEXTURES_HH
#define TEXTURES_HH

#include "image.hh"
#include "point.hh"

#include <vector>
#include <filesystem>
using namespace std;

class Textures
{
private:
    int height, width;

    vector<vector<vector<uint8_t>>> colorMap;

    vector<Point<int>> drawnPoints;

public:
    Textures(int width, int height);

    void drawPoints(const vector<Point<int>>& points, vector<uint8_t> color);

    void drawCircles(const vector<Point<int>>& points, vector<uint8_t> color, int radius);

    vector<Point<int>> getDrawnPoints() const;

    void save(string dirname = ".");
};

#endif