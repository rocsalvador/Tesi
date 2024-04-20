#include "kmeans.hh"

#include <iostream>

KMeans::KMeans(const vector<Point<int>>& points, int k)
{
    this->k = k;
    this->clusters = vector<vector<Point<int>>> (k);
    this->centroids = vector<Point<int>> (k);
    this->clustersQueues = vector<queue<Point<int>>> (k);
    this->points = points; 
    this->maxValue = 0;
    for (const Point<int>& point : this->points)
    {
        this->maxValue = max(maxValue, max(point.getX(), point.getY()));
    }
    this->map = vector<vector<bool>> (maxValue + 1, vector<bool> (maxValue + 1, false));
}

void KMeans::assignClusters()
{
    for (const Point<int>& point : this->points)
    {
        this->map[point.getX()][point.getY()] = true;
    }

    for (int i = 0; i < k; ++i)
    {
        clustersQueues[i].push(centroids[i]);
    }

    bool allQueuesEmpty = false;
    while (not allQueuesEmpty)
    {
        allQueuesEmpty = true;
        for (int i = 0; i < k; ++i)
        {
            if (clustersQueues[i].size() != 0)
            {
                allQueuesEmpty = false;
                Point<int> point = clustersQueues[i].front();
                clustersQueues[i].pop();

                int x = point.getX(), y = point.getY();
                if (map[x][y])
                {
                    clusters[i].push_back(point);
                    map[x][y] = false;

                    if (x > 0 and map[x - 1][y])
                        clustersQueues[i].push(Point<int>(x - 1, y));
                    if (y > 0 and map[x][y - 1])
                        clustersQueues[i].push(Point<int>(x, y - 1));
                    if (x < maxValue and map[x + 1][y])
                        clustersQueues[i].push(Point<int>(x + 1, y));
                    if (y < maxValue and map[x][y + 1])
                        clustersQueues[i].push(Point<int>(x, y + 1));
                }
            } 
        }
    }
}

void KMeans::computeCentroids()
{

}

void KMeans::run(int maxIt)
{
    cout << "[KMeans] Starting KMeans with max it " << maxIt << endl;
    // Initialize centroids in random points
    for (int i = 0; i < k; ++i)
    {
        int randIdx = rand() / double(RAND_MAX) * (points.size() - 1);
        centroids[i] = points[randIdx]; 
    }
    assignClusters();

    for (int i = 0; i < maxIt; ++i)
    {
        computeCentroids();
        assignClusters();
    }
}

vector<vector<Point<int>>> KMeans::getClusters()
{
    return clusters;
}

vector<Point<int>> KMeans::getCentroids()
{
    return centroids;
}

