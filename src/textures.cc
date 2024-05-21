#include "textures.hh"

#include <iostream>

Textures::Textures(int width, int height)
{
    this->height = height;
    this->width = width;

    drawnMap = vector<vector<bool>> (height, vector<bool> (width, false));
    colorMap = vector<vector<vector<uint8_t>>> (height, vector<vector<uint8_t>> (width, vector<uint8_t> (3, 0)));
    normalMap = vector<vector<vector<uint8_t>>> (height, vector<vector<uint8_t>> (width, vector<uint8_t> (3, 0)));
}

Textures::Textures() {}

vector<double> Textures::computeNormal(const Point<int>& point, const Point<int>& center, int radius)
{
    // int i = point.getX();
    // int j = point.getY();
    // int x = center.getX();
    // int y = center.getY();
    // double normX = i - x;
    // double normY = j - y;
    // double normZ = sqrt(pow(radius, 2) - pow(Point<int>::euclidianDistance(point, center), 2));
    // double unitFactor = sqrt(pow(normX, 2) + pow(normY, 2) + pow(normZ, 2));
    // return {normX / unitFactor, normY / unitFactor, normZ / unitFactor};
    double factor = Point<int>::euclidianDistance(point, center) / radius * 3;
    double red = factor, green = 0, blue = 0;
    if (factor > 1)
    {
        if (factor > 2)
        {
            red = 1;
            green = 1;
            blue = factor - 2;
        }
        else 
        {
            red = 1;
            green = factor - 1;
        }
    }
    return {red, green, blue};
}

void Textures::drawCircle(const Point<int>& point, vector<uint8_t> color, int radius, bool computeNormals)
{
    assert(point.getX() < width and point.getY() < height);
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

                if (computeNormals)
                {
                    vector<double> normals = computeNormal({i, j}, point, radius);
                    normalMap[i][j] = {uint8_t(63.5 + normals[0] * 191.5),
                                       uint8_t(63.5 + normals[1] * 191.5),
                                       uint8_t(63.5 + normals[2] * 191.5)};
                }
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
    
                if (computeNormals)
                {
                    vector<double> normals = computeNormal({i_inv, j}, point, radius);
                    normalMap[i_inv][j] = {uint8_t(63.5 + normals[0] * 191.5),
                                       uint8_t(63.5 + normals[1] * 191.5),
                                       uint8_t(63.5 + normals[2] * 191.5)};                }
            }

            ++i;
            distance = Point<int>::euclidianDistance({x, y}, {i, j});

        }
    }
}

void Textures::drawCircles(const vector<Point<int>>& points, vector<uint8_t> color, int radius, bool computeNormals)
{
    for (const Point<int>& point : points)
    {
        drawCircle(point, color, radius, computeNormals);
    }
}

void Textures::drawLine(Point<int> a, Point<int> b, vector<uint8_t> color)
{
    int x1 = a.getX(), y1 = a.getY();
    int x2 = b.getX(), y2 = b.getY();

    int dx = x2 - x1; 
    int dy = y2 - y1; 
  
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy); 
  
    float xinc = dx / (float)steps; 
    float yinc = dy / (float)steps; 
  
    float x = x1; 
    float y = y1; 
    for (int i = 0; i <= steps; i++) { 
        if (not drawnMap[x][y])
        {
            drawnPoints.push_back({x, y});
            drawnMap[x][y] = true;
        }
        colorMap[x][y] = color;
        x += xinc;
        y += yinc;
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

    int mapSize;
    centersFile >> mapSize;
    this->height = mapSize;
    this->width = mapSize;

    drawnMap = vector<vector<bool>> (height, vector<bool> (width, false));
    colorMap = vector<vector<vector<uint8_t>>> (height, vector<vector<uint8_t>> (width, vector<uint8_t> (3, 0)));
    normalMap = vector<vector<vector<uint8_t>>> (height, vector<vector<uint8_t>> (width, vector<uint8_t> (3, 0)));

    int x, y;
    while (centersFile >> x and centersFile >> y)
    {
        drawCircle({x, y}, color, radius);
    }
}

void Textures::drawPoints(const vector<Point<int>>& points, vector<uint8_t> color)
{
    for (const Point<int>& point : points)
    {
        drawPoint(point, color);
    }
}

void Textures::drawPoint(const Point<int>& point, vector<uint8_t> color)
{
    if (point.getX() >= height or point.getX() < 0 or point.getY() >= width or point.getY() < 0)
        return;
    colorMap[point.getX()][point.getY()] = color;
}

const vector<Point<int>>& Textures::getDrawnPoints()
{
    return drawnPoints;
}

int Textures::getHeight() const
{
    return height;
}

vector<uint8_t> Textures::getNormal(const Point<int>& point)
{
    int x = point.getX();
    int y = point.getY();
    return normalMap[x][y];
}

void Textures::save(string dirname, string name)
{
    if (not filesystem::exists(dirname))
    {
        filesystem::create_directory(dirname);
    }

    Image colorImg;
    filesystem::path fullPath = filesystem::path(dirname) / filesystem::path(name + "color_map.png");
    colorImg.save(colorMap, fullPath);

    Image normalImg;
    fullPath = filesystem::path(dirname) / filesystem::path(name + "normal_map.png");
    normalImg.save(normalMap, fullPath);
}

void Textures::clear()
{
    colorMap = vector<vector<vector<uint8_t>>> (height, vector<vector<uint8_t>> (width, vector<uint8_t> (3, 0)));
    normalMap = vector<vector<vector<uint8_t>>> (height, vector<vector<uint8_t>> (width, vector<uint8_t> (3, 0)));
    drawnMap = vector<vector<bool>> (height, vector<bool> (width, false));
}