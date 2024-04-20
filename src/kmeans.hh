#ifndef KMEANS_HH
#define KMEANS_HH

#include "point.hh"

#include <vector>
#include <queue>
using namespace std;

class KMeans 
{
private:
    int k;
    int maxValue;

    vector<Point<int>> points;

    vector<Point<int>> centroids;

    vector<vector<Point<int>>> clusters;

    vector<queue<Point<int>>> clustersQueues;

    vector<vector<bool>> map;

    void assignClusters();

    void computeCentroids();

public:
    KMeans(const vector<Point<int>>& points, int k);

    void run(int maxIt);

    vector<vector<Point<int>>> getClusters();

    vector<Point<int>> getCentroids();
};

#endif