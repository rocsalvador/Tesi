#ifndef POINT_HH
#define POINT_HH

#include <cmath>
#include <ostream>
using namespace std;

template<class T>
class Point
{
private:
    T x, y;

public:
    template<class T2>
    Point(const Point<T2> &t2)
    {
        this->x = t2.getX();
        this->y = t2.getY();
    }

    void operator=(pair<T, T> pair);

    bool operator==(const Point<T>& p2) const;

    void operator+=(const Point<T>& p2);

    bool operator!=(const Point<T>& p2) const;

    Point<T> operator*(T multiplier) const;

    Point<T> operator+(const Point<T>& p) const;

    Point(pair<T, T> p);

    Point();

    Point(T x, T y);

    T getX() const;

    T getY() const;

    void setX(T x);

    void setY(T y);

    static double euclidianDistance(const Point<T>& p1, const Point<T>& p2);

    static double sqEuclidianDistance(const Point<T>& p1, const Point<T>& p2);
};

#endif