#include "kmeans.hh"

#include "textures.hh"
#include <iostream>

KMeans::KMeans(const vector<Point<int>>& paramPoints, int k, const vector<Point<int>>& centroids)
{
    this->k = k;
    this->clusters = vector<vector<Point<int>>> (k);
    this->points = paramPoints;
    if (centroids.size() != k)
    {
        // Initialize centroids in random points
        this->centroids = vector<Point<int>> (k);
        for (int i = 0; i < k; ++i)
        {
            int randIdx = rand() / double(RAND_MAX) * (points.size() - 1);
            this->centroids[i] = points[randIdx]; 
        }
    }
    else    
        this->centroids = centroids;
    this->clustersQueues = vector<priority_queue<pair<double, Point<int>>, vector<pair<double, Point<int>>>, comp>> (k);
    this->clustersEdges = vector<vector<Point<int>>> (k);
    this->maxValue = 0;
    for (const Point<int>& point : this->points)
        this->maxValue = max(maxValue, max(point.getX(), point.getY()));
    this->map = vector<vector<bool>> (maxValue + 1, vector<bool> (maxValue + 1, false));
    clustersColor = vector<vector<uint8_t>> (k, vector<uint8_t> (3));
    for (uint i = 0; i < k; ++i)
        clustersColor[i] = {uint8_t(rand() / double(RAND_MAX) * 255),
                            uint8_t(rand() / double(RAND_MAX) * 255),
                            uint8_t(rand() / double(RAND_MAX) * 255)};
}

void KMeans::assignClusters()
{
    clusters = vector<vector<Point<int>>> (k);
    map = vector<vector<bool>> (maxValue + 1, vector<bool> (maxValue + 1, false));
    for (const Point<int>& point : points)
    {
        map[point.getX()][point.getY()] = true;
    }

    for (int i = 0; i < k; ++i)
    {
        clustersQueues[i].push({0, centroids[i]});
    }

    bool allQueuesEmpty = false;
    double maxDistance = 1;
    while (not allQueuesEmpty)
    {
        allQueuesEmpty = true;
        for (int i = 0; i < k; ++i)
        {
            while (clustersQueues[i].size() > 0)
            {
                allQueuesEmpty = false;
                Point<int> point = clustersQueues[i].top().second;
                double d = clustersQueues[i].top().first;
                if (d > maxDistance)
                    break;
                clustersQueues[i].pop();
                int x = point.getX();
                int y = point.getY();
                if (map[x][y])
                {
                    map[x][y] = false;
                    clusters[i].push_back(point);

                    for (uint j = max(x - 1, 0); j <= min(maxValue, x + 1); ++j)
                    {
                        for (uint l = max(y - 1, 0); l <= min(maxValue, y + 1); ++l)
                        {
                            if (map[j][l])
                            {
                                double off = 1;
                                if (((j == x + 1 or j == x - 1) and (l == y - 1 or l == y + 1)))
                                    off = 1.41421356237;
                                clustersQueues[i].push({d + off, {j, l}});
                            }
                        }
                    }
                }
            }

        }
        ++maxDistance;
    }
}

double KMeans::clusterCircleRatio(int i)
{
    double area = clusters[i].size();
    double perimeter = clustersEdges[i].size();
    return 4 * M_PI * area / (perimeter * perimeter);
}

void KMeans::computeEdges(uint i)
{
    clustersEdges[i] = vector<Point<int>> (0);
    vector<vector<bool>> clusterMap(maxValue + 1, vector<bool> (maxValue + 1, false));
    for (const Point<int>& point : clusters[i])
        clusterMap[point.getX()][point.getY()] = true;
    for (const Point<int>& point : clusters[i])
    {
        int x = point.getX();
        int y = point.getY();
        if (x > 0 and not clusterMap[x  - 1][y])
        {
            clustersEdges[i].push_back(point);
            continue;
        }
        if (x < maxValue and not clusterMap[x  + 1][y])
        {
            clustersEdges[i].push_back(point);
            continue;
        }
        if (y > 0 and not clusterMap[x][y - 1])
        {
            clustersEdges[i].push_back(point);
            continue;
        }
        if (y < maxValue and not clusterMap[x][y + 1])
        {
            clustersEdges[i].push_back(point);
            continue;
        }
    }
}

void KMeans::computeCentroids()
{
    for (uint i = 0; i < k; ++i)
    {
        computeEdges(i);

        double maxDistance = 0;
        Point<int> centroid;
        for (const Point<int>& point : clusters[i])
        {
            double minDistance = numeric_limits<double>::max();
            for (const Point<int>& edge : clustersEdges[i])
                minDistance = min(minDistance, Point<int>::euclidianDistance(point, edge));
            
            if (minDistance > maxDistance)
            {
                centroid = point;
                maxDistance = minDistance;
            }
        }
        centroids[i] = centroid;
    }
}

bool KMeans::areVectorsEqual(const vector<Point<int>>& v1, const vector<Point<int>>& v2)
{
    for (uint i = 0; i < k; ++i)
        if (v1[i] != v2[i])
            return false;
    return true;
}

void KMeans::runIteration()
{
    assignClusters();

    oldCentroids = centroids;

    computeCentroids();
}

void KMeans::run(int maxIt)
{
    runIteration();

    int i = 1;
    Textures textures(maxValue + 1, maxValue + 1);
    while (i < maxIt and not areVectorsEqual(centroids, oldCentroids))
    {
        runIteration();
        ++i;
    }
}

void KMeans::run(int maxIt, double minCircularity)
{
    runIteration();

    bool areClustersCircles = true;
    for (uint j = 0; j < k; ++j)
        areClustersCircles = areClustersCircles and (clusterCircleRatio(j) >= minCircularity);
    int i = 1;
    while (i < maxIt and (not areClustersCircles) and (not areVectorsEqual(centroids, oldCentroids)))
    {
        runIteration();
        
        areClustersCircles = true;
        for (uint j = 0; j < k; ++j)
            areClustersCircles = areClustersCircles and (clusterCircleRatio(j) >= minCircularity);

        ++i;
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

