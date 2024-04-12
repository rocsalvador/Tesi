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
    pair<double, double> boundingCircleCenter;

    double eulcidianDistance(const pair<double, double>& p0, const pair<double, double>& p1);

    double aggregationProbability(const pair<double, double> point);

    vector<pair<double, double>> cluster;

    pair<double, double> getRandomPoint();

    bool aggregatePoint(pair<double, double>& point);

public:
    DLA(uint mapSize, double radius, double alpha, double sigma, double tau, double p);

    void run(uint nPoints, string outFileName);
};