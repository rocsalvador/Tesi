#include "dla.hh"
#include <iostream>

DLA::DLA(double radius, double alpha, double sigma, double tau, double p)
{
    this->radius = radius;
    this->aggregateDistance = radius;
    this->moveDistance = radius;

    this->countDistance = p * radius / 1.5;
    this->alpha = alpha;
    this->sigma = sigma;
    this->tau = tau;
    this->p = p;
    
    this->boundingCircleRadius = 4 * radius;
    this->boundingCircleCenter = Point<double> (0, 0); 
    this->maxCoord = 0;
    this->generationDistance = boundingCircleRadius * 1.1;
    this->deathRadius = boundingCircleRadius * 1.2;
}

double DLA::aggregationProbability(const Point<double> point)
{
    uint count = 0;
    for (const Point<double>& aggregatedPoint : cluster)
        if (Point<double>::euclidianDistance(point, aggregatedPoint) <= countDistance)
            ++count;
    return alpha + (1 - alpha) * pow(exp(1.0), - sigma * pow(count - tau, 2));
}

Point<double> DLA::getRandomPoint()
{
    double angle = 2 * M_PI * rand() / double(RAND_MAX);
    double x = cos(angle) * generationDistance + boundingCircleCenter.getX();
    double y = sin(angle) * generationDistance + boundingCircleCenter.getY();

    return {x, y};
}

double DLA::getMaxCoord() const
{
    return maxCoord;
}

bool DLA::aggregatePoint(Point<double>& point)
{
    while (true)
    {
        for (const Point<double>& aggregatedPoint : cluster)
        {
            if (Point<double>::euclidianDistance(point, aggregatedPoint) <= aggregateDistance)
            {
                double prob = aggregationProbability(point);
                double r = rand() / double(RAND_MAX);
                if (r <= prob)
                {
                    cluster.push_back(point);
                    maxCoord = max(max(abs(point.getX()), maxCoord), abs(point.getY()));
                    double centerDistance = Point<double>::euclidianDistance(point, boundingCircleCenter);
                    if (centerDistance > boundingCircleRadius)
                    {
                        boundingCircleRadius = centerDistance;
                        generationDistance = boundingCircleRadius * 1.1;
                        deathRadius = boundingCircleRadius * 1.2;
                    }
                    return true;
                }
                else
                    return false;
            }
        }

        double angle = 2 * M_PI * rand() / double(RAND_MAX);
        double x = cos(angle) * moveDistance + point.getX();
        double y = sin(angle) * moveDistance + point.getY();

        point = Point(x, y);

        if (Point<double>::euclidianDistance(point, boundingCircleCenter) > deathRadius)
            return false;
    }
}

void DLA::run(uint nPoints)
{
    cout << "[DLA] Starting dla with " << nPoints << " points" << endl;
    cluster.push_back(boundingCircleCenter);

    uint tenth = nPoints / 10;

    for (uint i = 1; i <= nPoints; ++i)
    {
        Point<double> point = getRandomPoint();
        while (not aggregatePoint(point))
        {
            point = getRandomPoint();
        }

        if (i % tenth == 0)
            cout << "[DLA] Added " << i << " points" << endl;
    }
}

vector<Point<double>> DLA::getPoints() const
{
    return cluster;
}
