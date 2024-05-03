#include "textures.hh"

#include <iostream>

Textures::Textures(int width, int height)
{
    this->height = height;
    this->width = width;

    colorMap = vector<vector<vector<uint8_t>>> (height, vector<vector<uint8_t>> (width, vector<uint8_t> (3, 0)));
    drawnMap = vector<vector<bool>> (height, vector<bool> (width, false));
}

void Textures::drawCircle(const Point<int>& point, vector<uint8_t> color, int radius)
{
    int x = point.getX();
    int y = point.getY();
    for (int j = min(height - 1, y + radius); j >= max(y - radius, 0); --j)
    {
        int i = x;
        double distance = Point<int>::euclidianDistance({x, y}, {i, j});
        while (distance <= radius)
        {
            if (i < width)
            {
                if (not drawnMap[i][j])
                {
                    drawnPoints.push_back({i, j});
                    drawnMap[i][j] = true;
                }
                colorMap[i][j] = color;
            }
            
            int i_inv = i - 2 * (i - x);
            if (i_inv >= 0)
            {
                if (not drawnMap[i_inv][j])
                {
                    drawnPoints.push_back({i_inv, j});
                    drawnMap[i_inv][j] = true;
                }
                colorMap[i_inv][j] = color;
            }

            ++i;
            distance = Point<int>::euclidianDistance({x, y}, {i, j});

        }
    }
}

void Textures::drawCircles(const vector<Point<int>>& points, vector<uint8_t> color, int radius)
{
    for (const Point<int>& point : points)
    {
        drawCircle(point, color, radius);
    }
}

void Textures::readSphereCenterPoints(string filename,  vector<uint8_t> color, int radius)
{
    ifstream centersFile(filename);
    if (not centersFile.is_open())
    {
        cerr << "[ERROR] Can not open centers points file " << filename << endl;
        return;
    }

    double x, y;
    while (centersFile >> x and centersFile >> y)
    {
        drawCircle({x, y}, color, radius);
    }
}

void Textures::drawPoints(const vector<Point<int>>& points, vector<uint8_t> color)
{
    for (const Point<int>& point : points)
    {
        colorMap[point.getX()][point.getY()] = color;
    }
}

const vector<Point<int>>& Textures::getDrawnPoints()
{
    return drawnPoints;
}

void Textures::save(string dirname, string name)
{
    Image colorImg;
    if (not filesystem::exists(dirname))
    {
        filesystem::create_directory(dirname);
    }
    filesystem::path fullPath = filesystem::path(dirname) / filesystem::path(name + "color_map.png");
    colorImg.save(colorMap, fullPath);
}

void Textures::clear()
{
    colorMap = vector<vector<vector<uint8_t>>> (height, vector<vector<uint8_t>> (width, vector<uint8_t> (3, 0)));
    drawnMap = vector<vector<bool>> (height, vector<bool> (width, false));
}