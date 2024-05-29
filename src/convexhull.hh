#ifndef CONVEXHULL_HH
#define CONVEXHULL_H

#include "point.hh" 

#include <vector>
#include <stack>
#include <algorithm>
#include <cmath>
#include <limits>
using namespace std;

class ConvexHull
{
private:
    double minDistance;
    double perimeter;
    vector<Point<int>> points;
    Point<int> p0;
    vector<Point<int>> contour;
    vector<double> sideLength;

    int orientation(Point<int> p, Point<int> q, Point<int> r);

    bool compare(const Point<int>& p1, const Point<int>& p2);

    void swap(Point<int> &p1, Point<int> &p2);

    double angle(const Point<int> &p1, const Point<int> &p2, const Point<int> &p3);

    Point<int> nextToTop(stack<Point<int>> &S);

public:
    ConvexHull();

    ConvexHull(const vector<Point<int>>& points);

    vector<Point<int>> getContour();

    void computeConvexHull();

    void computeBorder(uint samples, uint mapSize);

    void setContour(const vector<Point<int>>& contour);

    vector<double> computeMVC(const Point<int>& point);

    double getSideLength(uint i) const;

    double getPerimeter() const;
};

#endif