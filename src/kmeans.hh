#ifndef KMEANS_HH
#define KMEANS_HH

#include "point.hh"

#include <vector>
#include <queue>
#include <stack>
#include <limits>
using namespace std;

struct comp {
    constexpr bool operator()(
        pair<double, Point<int>> const& a,
        pair<double, Point<int>> const& b)
        const noexcept
    {
        return a.first > b.first;
    }
};
 

class KMeans 
{
private:
    int k;
    int maxValue;
    int currentIt;

    vector<Point<int>> points;

    vector<Point<int>> centroids;

    vector<Point<int>> oldCentroids;

    vector<vector<Point<int>>> clusters;

    vector<priority_queue<pair<double, Point<int>>, vector<pair<double, Point<int>>>, comp>>  clustersQueues;

    vector<vector<Point<int>>> clustersEdges;

    vector<pair<Point<int>, Point<int>>> clustersBoundingBoxes;

    vector<vector<bool>> map;

    vector<double> clustersRatios;

    void assignClusters();

    void computeCentroids();

    void computeEdges(uint i);

    bool areVectorsEqual(const vector<Point<int>>& v1, const vector<Point<int>>& v2);

    void runIteration();

    void computeClusterRatio(uint i);

public:
    KMeans(const vector<Point<int>>& points, int k,  const vector<Point<int>>& centroids = vector<Point<int>> ());

    double clusterCircleRatio(int i);

    void run(int maxIt);

    void run(int maxIt, double minCircularity);

    vector<vector<Point<int>>> getClusters();

    vector<Point<int>> getCentroids();

    vector<Point<int>> getSortedEdges(uint i);
};

#endif