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
Point<T>::Point(pair<T, T> p)
{
    this->x = p.first;
    this->y = p.second;
}

template<class T>
bool Point<T>::operator==(const Point<T>& p2) const
{
    return (this->getX() == p2.getX()) and (this->getY() == p2.getY());
}

template<class T>
void Point<T>::operator+=(const Point<T>& p2)
{
    x += p2.getX();
    y += p2.getY();
}


template<class T>
bool Point<T>::operator!=(const Point<T>& p2) const
{
    return (not (this->getX() == p2.getX())) or (not (this->getY() == p2.getY()));
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
double Point<T>::euclidianDistance(const Point<T>& p1, const Point<T>& p2)
{
    double distance = sqrt(sqEuclidianDistance(p1, p2));
    return distance;
}

template<class T>
double Point<T>::sqEuclidianDistance(const Point<T>& p1, const Point<T>& p2)
{
    double distance = pow(p1.getX() - p2.getX(), 2) + pow(p1.getY() - p2.getY(), 2);
    return distance;
}