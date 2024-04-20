#include "point.hh"

template class Point<double>;
template class Point<float>;
template class Point<uint>;
template class Point<int>;

template<class T>
void Point<T>::operator=(pair<T, T> pair)
{
    this->x = pair.first;
    this->y = pair.second;
}

template<class T>
Point<T>::Point() {}

template<class T>
Point<T>::Point(T x, T y)
{
    this->x = x;
    this->y = y;
}

template<class T>
T Point<T>::getX() const
{
    return x;
}

template<class T>
T Point<T>::getY() const
{
    return y;
}

template<class T>
void Point<T>::setX(T x)
{
    this->x = x;
}

template<class T>
void Point<T>::setY(T y)
{
    this->y = y;
}

template<class T>
double Point<T>::euclidianDistance(Point<T> p1, Point<T> p2)
{
    double distance = sqrt(pow(p1.getX() - p2.getX(), 2) + pow(p1.getY() - p2.getY(), 2));
    return distance;
}