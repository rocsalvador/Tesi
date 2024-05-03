#ifndef KMEANS_HH
#define KMEANS_HH

#include "point.hh"

#include <vector>
#include <queue>
#include <limits>
using namespace std;

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

    vector<queue<Point<int>>> clustersQueues;

    vector<vector<Point<int>>> clustersEdges;

    vector<vector<bool>> map;

    vector<vector<uint8_t>> clustersColor;

    void assignClusters();

    void computeCentroids();

    void computeEdges();

    bool areVectorsEqual(const vector<Point<int>>& v1, const vector<Point<int>>& v2);

    void runIteration();

public:
    KMeans(const vector<Point<int>>& points, int k,  const vector<Point<int>>& centroids = vector<Point<int>> ());

    double clusterCircleRatio(int i);

    void run(int maxIt);

    vector<vector<Point<int>>> getClusters();

    vector<Point<int>> getCentroids();
};

#endif