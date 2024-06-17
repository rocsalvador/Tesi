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
    this->clustersRatios = vector<double> (k);
}

void KMeans::assignClusters()
{
    clustersBoundingBoxes = vector<pair<Point<int>, Point<int>>>(k, {{maxValue + 1, maxValue + 1}, {0, 0}});
    clusters = vector<vector<Point<int>>> (k);
    map = vector<vector<bool>> (maxValue + 1, vector<bool> (maxValue + 1, false));
    vector<vector<bool>> ogMap(maxValue + 1, vector<bool> (maxValue + 1, false));

    for (const Point<int>& point : points)
    {
        map[point.getX()][point.getY()] = true;
        ogMap[point.getX()][point.getY()] = true;
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
                int x = point.getX();
                int y = point.getY();
                clustersQueues[i].pop();
                if (map[x][y])
                {
                    map[x][y] = false;
                    clusters[i].push_back(point);
                    clustersBoundingBoxes[i] = {{min(clustersBoundingBoxes[i].first.getX(), point.getX()),
                                                 min(clustersBoundingBoxes[i].first.getY(), point.getY())},
                                                {max(clustersBoundingBoxes[i].second.getX(), point.getX()),
                                                 max(clustersBoundingBoxes[i].second.getY(), point.getY())}};

                    for (uint j = max(x - 1, 0); j <= min(maxValue, x + 1); ++j)
                    {
                        for (uint l = max(y - 1, 0); l <= min(maxValue, y + 1); ++l)
                        {
                            if (map[j][l])
                            {
                                double off = 1;
                                if (((j == x + 1 or j == x - 1) and (l == y - 1 or l == y + 1)))
                                {
                                    if (not map[x][l] and not map[j][y])
                                        continue;
                                    off = 1.41421356237;
                                }
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

vector<Point<int>> KMeans::getSortedEdges(uint i)
{
    vector<vector<bool>> clusterMap(maxValue + 1, vector<bool>(maxValue + 1, false));
    for (const Point<int> &point : clustersEdges[i])
        clusterMap[point.getX()][point.getY()] = true;

    bool allVisited = false;
    Point<int> currentPoint = clustersEdges[i][0];
    vector<Point<int>> sortedEdges(clustersEdges[i].size());
    uint k = 0;
    sortedEdges[k] = currentPoint;
    clusterMap[currentPoint.getX()][currentPoint.getY()] = false;

    stack<int> S;

    while (not allVisited)
    {
        int x = currentPoint.getX(), y = currentPoint.getY();
        bool foundEdge = false;
        bool pushed = false;
        for (uint j = max(x - 1, 0); not pushed and j <= min(maxValue, x + 1); ++j)
        {
            for (uint l = max(y - 1, 0); not pushed and l <= min(maxValue, y + 1); ++l)
            {
                if (clusterMap[j][l])
                {
                    if (foundEdge)
                    {
                        S.push(k - 1);
                        pushed = true;
                    }
                    else 
                    {
                        ++k;
                        currentPoint = Point<int>(j, l);
                        sortedEdges[k] = currentPoint;
                        foundEdge = true;
                        clusterMap[j][l] = false;
                    }
                }
            }
        }
        
        if (not foundEdge and not allVisited)
        {
            if (S.empty())
                allVisited = true;
            else
            {
                currentPoint = sortedEdges[S.top()];
                S.pop();
            }
        }
    }
    return sortedEdges;
}


void KMeans::computeClusterRatio(uint i)
{
    double area = clusters[i].size();
    double perimeter = clustersEdges[i].size();
    clustersRatios[i] = 4 * M_PI * area / (perimeter * perimeter);
}

double KMeans::clusterCircleRatio(int i)
{
    return clustersRatios[i];
}

void KMeans::computeEdges(uint i)
{
    clustersEdges[i] = vector<Point<int>>(0);
    if (clusters[i].size() == 0)
        return;
    int clusterLengthX = clustersBoundingBoxes[i].second.getX() - clustersBoundingBoxes[i].first.getX() + 3;
    int clusterLengthY = clustersBoundingBoxes[i].second.getY() - clustersBoundingBoxes[i].first.getY() + 3;
    int offsetX = clustersBoundingBoxes[i].first.getX();
    int offsetY = clustersBoundingBoxes[i].first.getY();
    vector<vector<bool>> clusterMap(clusterLengthX, vector<bool>(clusterLengthY, false));
    vector<vector<bool>> visitedMap(clusterLengthX, vector<bool>(clusterLengthY, false));
    for (const Point<int> &point : clusters[i])
    {
        clusterMap[point.getX() + 1 - offsetX][point.getY() + 1 - offsetY] = true;
    }
    Point<int> currentPoint = Point<int>(0, 0);
    queue<Point<int>> Q;
    Q.push(currentPoint);
    while (not Q.empty())
    {
        Point<int> top = Q.front();
        Q.pop();
        int x = top.getX();
        int y = top.getY();
        if (x > 0 and not visitedMap[x - 1][y])
        {
            if (clusterMap[x - 1][y])
                clustersEdges[i].push_back({x - 2 + offsetX, y - 1 + offsetY});
            else
                Q.push({x - 1, y});
            visitedMap[x - 1][y] = true;
        }
        if (x < clusterLengthX - 1 and not visitedMap[x + 1][y])
        {
            if (clusterMap[x + 1][y])
                clustersEdges[i].push_back({x + offsetX, y - 1 + offsetY});
            else
                Q.push({x + 1, y});
            visitedMap[x + 1][y] = true;
        }
        if (y > 0 and not visitedMap[x][y - 1])
        {
            if (clusterMap[x][y - 1])
                clustersEdges[i].push_back({x - 1 + offsetX, y - 2 + offsetY});
            else
                Q.push({x, y - 1});
            visitedMap[x][y - 1] = true;
        }

        if (y < clusterLengthY - 1 and not visitedMap[x][y + 1])
        {

            if (clusterMap[x][y + 1])
                clustersEdges[i].push_back({x - 1 + offsetX, y + offsetY});
            else
                Q.push({x, y + 1});
            visitedMap[x][y + 1] = true;
        }
    }

    computeClusterRatio(i);
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
        areClustersCircles = areClustersCircles and (clustersRatios[j] >= minCircularity);
    int i = 1;
    while (i < maxIt and (not areClustersCircles) and (not areVectorsEqual(centroids, oldCentroids)))
    {
        runIteration();
        
        areClustersCircles = true;
        for (uint j = 0; j < k; ++j)
            areClustersCircles = areClustersCircles and (clustersRatios[j] >= minCircularity);

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

