#include "kmeans.hh"

#include "textures.hh"
#include <iostream>

KMeans::KMeans(const vector<Point<int>>& paramPoints, int k, const vector<Point<int>>& centroids)
{
    this->k = k;
    this->clusters = vector<vector<Point<int>>> (k);
    if (centroids.size() != k)
        this->centroids = vector<Point<int>> (k);
    else    
        this->centroids = centroids;
    this->clustersQueues = vector<queue<Point<int>>> (k);
    this->clustersEdges = vector<vector<Point<int>>> (k);
    this->points = paramPoints;
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
        clustersQueues[i].push(centroids[i]);
    }

    bool allQueuesEmpty = false;
    while (not allQueuesEmpty)
    {
        allQueuesEmpty = true;
        for (int i = 0; i < k; ++i)
        {
            queue<Point<int>> newQueue;
            while (clustersQueues[i].size() > 0)
            {
                allQueuesEmpty = false;
                Point<int> point = clustersQueues[i].front();
                clustersQueues[i].pop();
                clusters[i].push_back(point);
                int x = point.getX();
                int y = point.getY();

                if (x > 0 and map[x - 1][y])
                {
                    newQueue.push(Point<int>(x - 1, y));
                    map[x - 1][y] = false;
                }
                if (y > 0 and map[x][y - 1])
                {
                    newQueue.push(Point<int>(x, y - 1));
                    map[x][y - 1] = false;
                }
                if (x < maxValue and map[x + 1][y])
                {
                    newQueue.push(Point<int>(x + 1, y));
                    map[x + 1][y] = false;
                }
                if (y < maxValue and map[x][y + 1])
                {
                    newQueue.push(Point<int>(x, y + 1));
                    map[x][y + 1] = false;
                }
            }

            clustersQueues[i] = newQueue;
        }
    }
}

double KMeans::clusterCircleRatio(int i)
{
    double area = clusters[i].size();
    double perimeter = clustersEdges[i].size();
    return 4 * M_PI * area / (perimeter * perimeter);
}

void KMeans::computeEdges()
{
    for (uint i = 0; i < k; ++i)
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
}

void KMeans::computeCentroids()
{
    computeEdges();

    for (uint i = 0; i < k; ++i)
    {
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
    bool equal = true;
    for (uint i = 0; i < k; ++i)
    {
        if (v1[i] != v2[i])
        {
            equal = false;
            break;
        }
    }
    return equal;
}

void KMeans::runIteration()
{
    assignClusters();

    oldCentroids = centroids;

    computeCentroids();
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
    int tenth = maxIt / 10;

    runIteration();

    int i = 1;
    Textures textures(maxValue + 1, maxValue + 1);
    while (i < maxIt and not areVectorsEqual(centroids, oldCentroids))
    {
        runIteration();

        ++i;
        if (tenth > 0 and i % tenth == 0)
            cout << "[KMeans] Iteration " << i << endl;
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

