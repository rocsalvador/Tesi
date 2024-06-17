#ifndef TEXTURES_HH
#define TEXTURES_HH

#include "image.hh"
#include "point.hh"

#include <vector>
#include <filesystem>
#include <fstream>
#include <cassert>
#include <queue>
using namespace std;

class Textures
{
private:
    int height, width;
    int components;

    vector<vector<vector<uint8_t>>> colorMap;
    vector<vector<vector<uint8_t>>> normalMap;
    vector<vector<vector<uint8_t>>> displacementMap;

    vector<Point<int>> drawnPoints;

    vector<vector<bool>> drawnMap;

    vector<double> computeNormal(const Point<int>& point, const Point<int>& center, int radius);

public:
    Textures(int width, int height, int components = 3);

    Textures();

    /**
     * @brief Reads the provided txt filename where each line contains the x and the y
     * coordinates of the center point of the circle separated by a space
     * @param filename File name of the txt containing the centers coordinates
     * @param color Three length uint8_t array indicating the color of the circle to be
     * drawn
     * @param radius Radius of the sphere to be drawn
     * @pre 
     * @post colorMap, drawnPoints and drawnMap are updated with the new circle 
    */
    void readSphereCenterPoints(string filename, vector<uint8_t> color, int radius);

    void drawPoints(const vector<Point<int>>& points, vector<uint8_t> color);

    void drawPoint(const Point<int>& point, vector<uint8_t> color, vector<uint8_t> normal = vector<uint8_t> (0), vector<uint8_t> displacement = vector<uint8_t> (0));

    void drawCircle(const Point<int>& point, vector<uint8_t> color, int radius, bool computeNormals = false);

    void drawCircles(const vector<Point<int>>& points, vector<uint8_t> color, int radius, bool computeNormals = false);

    void fillShape(const Point<int>& center);

    void drawLine(Point<int> a, Point<int> b, vector<uint8_t> color);

    void scale(uint ratio);

    int getHeight() const;

    int getWidth() const;

    vector<uint8_t> getNormal(const Point<int>& point);

    vector<uint8_t> getColor(const Point<int>& point);

    vector<uint8_t> getDisplacement(const Point<int>& point);

    const vector<Point<int>>& getDrawnPoints();

    void save(string dirname = ".", string name = "");

    void clear();
};

#endif