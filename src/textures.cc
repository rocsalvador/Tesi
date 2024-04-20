#include "textures.hh"

#include <iostream>

Textures::Textures(int width, int height)
{
    this->height = height;
    this->width = width;

    colorMap = vector<vector<vector<uint8_t>>> (height, vector<vector<uint8_t>> (width, vector<uint8_t> (3, 0)));
}

void Textures::drawCircles(const vector<Point<int>>& points, vector<uint8_t> color, int radius)
{
    for (const Point<int>& point : points)
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
                    drawnPoints.push_back({i, j});
                    colorMap[i][j] = color;
                }
                
                int i_inv = i - 2 * (i - x);
                if (i_inv >= 0)
                {
                    drawnPoints.push_back({i_inv, j});
                    colorMap[i_inv][j] = color;
                }

                ++i;
                distance = Point<int>::euclidianDistance({x, y}, {i, j});

            }
        }
    }
}

void Textures::drawPoints(const vector<Point<int>>& points, vector<uint8_t> color)
{
    for (const Point<int>& point : points)
    {
        colorMap[point.getX()][point.getY()] = color;
    }
}

vector<Point<int>> Textures::getDrawnPoints() const
{
    return drawnPoints;
}

void Textures::save(string dirname)
{
    Image colorImg;
    filesystem::path fullPath = filesystem::path(dirname) / filesystem::path("color_map.png");
    colorImg.save(colorMap, fullPath);
}