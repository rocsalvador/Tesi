#include "dla.hh"
#include <iostream>

DLA::DLA(uint mapSize, double radius, double alpha, double sigma, double tau, double p)
{
    this->mapSize = mapSize;

    this->radius = radius;
    this->aggregateDistance = radius;
    this->moveDistance = radius;

    this->countDistance = p * radius / 1.5;
    this->alpha = alpha;
    this->sigma = sigma;
    this->tau = tau;
    this->p = p;
    
    this->boundingCircleRadius = 4 * radius;
    this->boundingCircleCenter = {mapSize / 2.0, mapSize / 2.0}; 
    this->generationDistance = boundingCircleRadius * 1.1;
    this->deathRadius = generationDistance * 1.1;
}

double DLA::eulcidianDistance(const pair<double, double>& p0, const pair<double, double>& p1)
{
    double distance = sqrt(pow(p0.first - p1.first, 2) + pow(p0.second - p1.second, 2));
    return distance;
}

double DLA::aggregationProbability(const pair<double, double> point)
{
    uint count = 0;
    for (const pair<double, double>& aggregatedPoint : cluster)
        if (eulcidianDistance(point, aggregatedPoint) <= countDistance)
            ++count;
    return alpha + (1 - alpha) * pow(exp(1.0), - sigma * pow(count - tau, 2));
}

pair<double, double> DLA::getRandomPoint()
{
    double angle = 2 * M_PI * rand() / double(RAND_MAX);
    double x = cos(angle) * generationDistance + boundingCircleCenter.first;
    double y = sin(angle) * generationDistance + boundingCircleCenter.second;

    while (x < 0 or x >= mapSize or y < 0 or y >= mapSize)
    {
        angle = 2 * M_PI * rand() / double(RAND_MAX);
        x = cos(angle) * generationDistance + boundingCircleCenter.first;
        y = sin(angle) * generationDistance + boundingCircleCenter.second;
    }
    return {x, y};
}

bool DLA::aggregatePoint(pair<double, double>& point)
{
    while (true)
    {
        for (const pair<double, double>& aggregatedPoint : cluster)
        {
            if (eulcidianDistance(aggregatedPoint, point) <= aggregateDistance)
            {
                double prob = aggregationProbability(point);
                double r = rand() / double(RAND_MAX);
                if (r <= prob)
                {
                    cluster.push_back(point);
                    double centerDistance = eulcidianDistance(point, boundingCircleCenter);
                    if (centerDistance > boundingCircleRadius)
                    {
                        boundingCircleRadius = centerDistance;
                        generationDistance = boundingCircleRadius * 1.1;
                        deathRadius =  generationDistance * 1.1;
                    }
                    return true;
                }
                else
                    return false;
            }
        }

        double angle = 2 * M_PI * rand() / double(RAND_MAX);
        double x = cos(angle) * moveDistance + point.first;
        double y = sin(angle) * moveDistance + point.second;

        while (x < 0 or x >= mapSize or y < 0 or y >= mapSize)
        {
            angle = 2 * M_PI * rand() / double(RAND_MAX);
            x = cos(angle) * moveDistance + point.first;
            y = sin(angle) * moveDistance + point.second;
        }

        point = {x, y};

        if (eulcidianDistance(point, boundingCircleCenter) > deathRadius)
            return false;
    }
}

void DLA::run(uint nPoints, string outFileName)
{
    double center = mapSize / 2.0;
    cluster.push_back({center, center});

    uint tenth = nPoints / 10;

    for (uint i = 1; i <= nPoints; ++i)
    {
        pair<double, double> point = getRandomPoint();
        while (not aggregatePoint(point))
        {
            point = getRandomPoint();
        }

        if (i % tenth == 0)
            cout << "Added " << i << " points" << endl;
    }

    ofstream outFile(outFileName);
    outFile << radius << endl;
    for (const pair<double, double>& point : cluster)
    {
        outFile << point.first << " " << point.second << endl;
    }
}
