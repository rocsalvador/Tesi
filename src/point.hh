#ifndef POINT_HH
#define POINT_HH

#include <cmath>
using namespace std;

template<class T>
class Point
{
private:
    T x, y;

public:
    void operator=(pair<T, T> pair);

    Point();

    Point(T x, T y);

    T getX() const;

    T getY() const;

    void setX(T x);

    void setY(T y);

    static double euclidianDistance(Point p1, Point p2);
};

#endif