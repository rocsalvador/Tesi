#ifndef DLA_HH
#define DLA_HH

#include "point.hh"

#include <vector>
#include <cstdlib>
#include <string>
#include <fstream>
#include <cmath>
#include <random>
using namespace std;

class DLA
{
private:
    uint mapSize;

    // Lichens behaviour parameters
    double radius;
    double generationDistance;
    double deathRadius;
    double aggregateDistance;
    double moveDistance;

    // Aggregation probability parameters
    double alpha;
    double tau;
    double sigma;
    double p;
    double countDistance;

    double boundingCircleRadius;
    Point<double> boundingCircleCenter;

    double aggregationProbability(const Point<double> point);

    vector<Point<double>> cluster;

    Point<double> getRandomPoint();

    bool aggregatePoint(Point<double>& point);

public:
    DLA(uint mapSize, double radius, double alpha, double sigma, double tau, double p);

    void run(uint nPoint);

    vector<Point<double>> getPoints() const;
};

#endif