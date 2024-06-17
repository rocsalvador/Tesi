#include "textures.hh"

#include <iostream>

Textures::Textures(int width, int height, int components)
{
    this->height = height;
    this->width = width;
    this->components = components;

    drawnMap = vector<vector<bool>> (height, vector<bool> (width, false));
    colorMap = vector<vector<vector<uint8_t>>> (height, vector<vector<uint8_t>> (width, vector<uint8_t> (components, 0)));
    normalMap = vector<vector<vector<uint8_t>>> (height, vector<vector<uint8_t>> (width, {0, 0, 0}));
    displacementMap = vector<vector<vector<uint8_t>>> (height, vector<vector<uint8_t>> (width, {0, 0, 0}));
}

Textures::Textures() {}

vector<double> Textures::computeNormal(const Point<int>& point, const Point<int>& center, int radius)
{
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
    int x = point.getX();
    int y = point.getY();
    for (int j = min(width - 1, y + radius); j >= max(y - radius, 0); --j)
    {
        int i = x;
        double distance = Point<int>::euclidianDistance({x, y}, {i, j});
        while (distance <= radius)
        {
            if (i < height)
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
    centersFile >> height >> width;

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

void Textures::drawPoint(const Point<int>& point, vector<uint8_t> color, vector<uint8_t> normal, vector<uint8_t> displacement)
{
    if (point.getX() >= height or point.getX() < 0 or point.getY() >= width or point.getY() < 0)
        return;    
    colorMap[point.getX()][point.getY()] = color;
    normalMap[point.getX()][point.getY()] = normal;
    displacementMap[point.getX()][point.getY()] = displacement;
    if (not drawnMap[point.getX()][point.getY()])
    {
        drawnMap[point.getX()][point.getY()] = true;
        drawnPoints.push_back(point);
    }
}

void Textures::scale(uint ratio)
{
    int newHeight = ratio * height, newWidth = width * ratio;
    vector<vector<bool>> newDrawnMap = vector<vector<bool>> (newHeight, vector<bool> (newWidth, false));
    vector<vector<vector<uint8_t>>> newColorMap = vector<vector<vector<uint8_t>>> (newHeight, vector<vector<uint8_t>> (newWidth, vector<uint8_t> (3, 0)));
    vector<vector<vector<uint8_t>>> newNormalMap = vector<vector<vector<uint8_t>>> (newHeight, vector<vector<uint8_t>> (newWidth, vector<uint8_t> (3, 0)));
    vector<Point<int>> newDrawnPoints; 

    for (const Point<int>& point : drawnPoints)
    {
        Point<int> basePoint = point * ratio;
        vector<uint8_t> currColor = colorMap[point.getX()][point.getY()];
        vector<uint8_t> currNormal = normalMap[point.getX()][point.getY()];
        for (uint i = 0; i < ratio; ++i)
        {
            for (uint j = 0; j < ratio; ++j)
            {
                Point<int> currPoint = basePoint + Point<int>(i, j);
                newDrawnPoints.push_back(currPoint);
                newDrawnMap[currPoint.getX()][currPoint.getY()] = true;
                newColorMap[currPoint.getX()][currPoint.getY()] = currColor;
                newNormalMap[currPoint.getX()][currPoint.getY()] = currNormal;
            }
        }
    }
    drawnMap = newDrawnMap;
    drawnPoints = newDrawnPoints;
    colorMap = newColorMap;
    normalMap = newNormalMap;
    height = newHeight;
    width = newWidth;
}

void Textures::fillShape(const Point<int>& center)
{
    vector<vector<bool>> visitedMap(height, vector<bool>(width, false));
    queue<Point<int>> Q;
    Q.push(center);
    while (not Q.empty())
    {
        Point<int> currentPoint = Q.front();
        Q.pop();
        int x = currentPoint.getX();
        int y = currentPoint.getY();
        if (visitedMap[x][y])
            continue;
        visitedMap[x][y] = true;
        drawPoint({x, y}, {255, 0, 0});
        if (x > 0 and not visitedMap[x - 1][y] and not drawnMap[x - 1][y])
            Q.push({x - 1, y});
        if (x < height - 1 and not visitedMap[x + 1][y] and not drawnMap[x + 1][y])
            Q.push({x + 1, y});
        if (y > 0 and not visitedMap[x][y - 1] and not drawnMap[x][y - 1])
            Q.push({x, y - 1});
        if (y < width - 1 and not visitedMap[x][y + 1] and not drawnMap[x][y + 1])
            Q.push({x, y + 1});
    }
}

const vector<Point<int>>& Textures::getDrawnPoints()
{
    return drawnPoints;
}

int Textures::getHeight() const
{
    return height;
}

int Textures::getWidth() const
{
    return width;
}

vector<uint8_t> Textures::getColor(const Point<int>& point)
{
    return colorMap[point.getX()][point.getY()];
}

vector<uint8_t> Textures::getNormal(const Point<int>& point)
{
    return normalMap[point.getX()][point.getY()];
}

vector<uint8_t> Textures::getDisplacement(const Point<int>& point)
{
    return displacementMap[point.getX()][point.getY()];
}


void Textures::save(string dirname, string name)
{
    if (not filesystem::exists(dirname))
    {
        filesystem::create_directory(dirname);
    }

    Image image;
    filesystem::path fullPath = filesystem::path(dirname) / filesystem::path(name + "color_map.png");
    image.save(colorMap, fullPath, components);

    fullPath = filesystem::path(dirname) / filesystem::path(name + "normal_map.png");
    image.save(normalMap, fullPath, 3);

    fullPath = filesystem::path(dirname) / filesystem::path(name + "displacement_map.png");
    image.save(displacementMap, fullPath, 3);

    vector<vector<vector<uint8_t>>> opacityMap = vector<vector<vector<uint8_t>>> (height, vector<vector<uint8_t>> (width, {0}));
    for (const Point<int>& point : drawnPoints)
        if (drawnMap[point.getX()][point.getY()])
            opacityMap[point.getX()][point.getY()][0] = 255;
    fullPath = filesystem::path(dirname) / filesystem::path(name + "opacity_map.png");
    image.save(opacityMap, fullPath, 1);
}

void Textures::clear()
{
    colorMap = vector<vector<vector<uint8_t>>> (height, vector<vector<uint8_t>> (width, vector<uint8_t> (3, 0)));
    normalMap = vector<vector<vector<uint8_t>>> (height, vector<vector<uint8_t>> (width, vector<uint8_t> (3, 0)));
    displacementMap = vector<vector<vector<uint8_t>>> (height, vector<vector<uint8_t>> (width, vector<uint8_t> (1, 0)));
    drawnMap = vector<vector<bool>> (height, vector<bool> (width, false));
    drawnPoints = vector<Point<int>>(0);
}