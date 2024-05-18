#include "dla.hh"
#include "textures.hh"
#include "convexhull.hh"
#include "point.hh"
#include "kmeans.hh"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <unordered_map>

string dtostr(double value)
{
    ostringstream oss;
    oss << setprecision(8) << noshowpoint << value;
    return oss.str();
}

string getField(const unordered_map<string, string>& configMap, string key)
{
    unordered_map<string, string>::const_iterator it = configMap.find(key);
    if (it != configMap.end())
        return it->second;
    cerr << "[ERROR] " << key << " not found in the config file" << endl;
    return "";
}

vector<Point<int>> runDla(const unordered_map<string, string>& configMap)
{
    int mapSize = stoi(getField(configMap, "img_size"));
    string centersFilename = getField(configMap, "centers_file");
    int radius = stoi(getField(configMap, "radius"));

    Textures textures(mapSize, mapSize);
    vector<uint8_t> color = {128, 128, 128};
    if (centersFilename == "-")
    {
        int points = stoi(getField(configMap, "points"));
        double alpha = stod(getField(configMap, "alpha"));
        double sigma = stod(getField(configMap, "sigma"));
        double tau = stod(getField(configMap, "tau"));
        double p = stod(getField(configMap, "p"));

        DLA dla(mapSize, radius, alpha, sigma, tau, p);
        dla.run(points);

        vector<Point<double>> dlaPoints = dla.getPoints();
        vector<Point<int>> dlaPointsInt = vector<Point<int>> (dlaPoints.begin(), dlaPoints.end());
        textures.drawCircles(dlaPointsInt, color, radius);

        cout << endl;
    }
    else 
        textures.readSphereCenterPoints(centersFilename, color, radius);
    return textures.getDrawnPoints();
}

vector<vector<Point<int>>> runKMeans(const unordered_map<string, string>& configMap, const vector<Point<int>>& drawnPoints)
{
    int maxIt = stoi(getField(configMap, "max_it"));
    int k = stoi(getField(configMap, "k"));
    int mapSize = stoi(getField(configMap, "img_size"));
    int minClusterSize = stoi(getField(configMap, "min_cluster_size"));
    double minCircularity = stod(getField(configMap, "min_circularity"));
    string imgDir = getField(configMap, "img_dir");

    KMeans kmeans(drawnPoints, k);
    kmeans.run(maxIt);

    vector<vector<Point<int>>> clusters = kmeans.getClusters();
    vector<Point<int>> centroids = kmeans.getCentroids();
    vector<double> clusterRatios (k);
    for (uint i = 0; i < k; ++i)
        clusterRatios[i] = kmeans.clusterCircleRatio(i);
    vector<bool> modifiedCluster(k, false);

    Textures textures(mapSize, mapSize);
    for (const vector<Point<int>>& cluster : clusters)
    {
        textures.drawPoints(cluster, {uint8_t(rand() / double(RAND_MAX) * 255),
                                      uint8_t(rand() / double(RAND_MAX) * 255),
                                      uint8_t(rand() / double(RAND_MAX) * 255)});
    }
    textures.drawPoints(centroids, {255, 0, 0});
    textures.save(imgDir, "first_");

    for (uint j = 0; j < k; ++j)
    {
        uint clusterSize = clusters[j].size();
        if (clusterSize > 1)
        {
            cout << "[KMeans] Cluster " << j << " ratio: " << clusterRatios[j] << endl; 
            double clusterRatio = clusterRatios[j];
            bool isCircle = clusterRatio >= minCircularity;
            modifiedCluster[j] = true;
            int it = 1;
            int newK = 2;
            int maxK = clusterSize / minClusterSize;
            while (maxK >= 2 and not isCircle)
            {
                if (it % 5 == 0)
                    ++newK;
                KMeans newKmeans = KMeans(clusters[j], newK);
                newKmeans.run(maxIt, minCircularity);
                isCircle = true;
                for (uint l = 0; l < newK and isCircle; ++l)
                    isCircle = isCircle and (newKmeans.clusterCircleRatio(l) >= minCircularity);
                isCircle = isCircle or (newK == maxK);
                if (isCircle) 
                {
                    vector<vector<Point<int>>> newClusters = newKmeans.getClusters();
                    vector<Point<int>> newCentroids = newKmeans.getCentroids();
                    centroids[j] = newCentroids[0];
                    clusters.insert(clusters.end(), newClusters.begin(), newClusters.end());
                    centroids.insert(centroids.end(), newCentroids.begin(), newCentroids.end());
                    cout << "[KMeans] Final clusters circularity:" << endl;
                    for (uint l = clusters.size() - newK - 1; l < clusters.size() and isCircle; ++l)
                        cout << "[KMeans] Cluster " << l << " ratio: " << newKmeans.clusterCircleRatio(l) << endl;
                    cout << endl;
                }
                ++it;
            }
        }
    }

    return clusters;
}


void textureParametrizaion(const unordered_map<string, string>& configMap, const vector<vector<Point<int>>>& clusters)
{
    int mapSize = stoi(getField(configMap, "img_size"));

    Textures textures(mapSize, mapSize);


    Textures circle(1000, 1000);
    Point<int> center = {500, 500};
    double radius = 400;
    circle.drawCircle(center, {255, 0, 0}, radius, true);
    for (uint i = 0; i < clusters.size(); ++i)
    {
        ConvexHull convexHull(clusters[i]);
        convexHull.computeConvexHull();
        vector<Point<int>> contour = convexHull.getContour();
        uint contourSize = contour.size();
        vector<double> perimeterProportion = vector<double> (contourSize);
        vector<double> sideLength = vector<double> (contourSize);
        double perimeter = 0;
        for (uint i = 0; i < contourSize; ++i)
        {
            sideLength[i] = Point<int>::euclidianDistance(contour[i], contour[(i + 1) % contourSize]);
            perimeter += sideLength[i];
        }

        vector<Point<int>> circleHull = vector<Point<int>> (contourSize);
        double currentAngle = 0;
        for (uint i = 0; i < contourSize; ++i)
        {
            int x = cos(currentAngle) * radius + center.getX();
            int y = sin(currentAngle) * radius + center.getY();
            circleHull[i] = Point<int>(x, y);
            currentAngle += (sideLength[i] / perimeter) * 2 * M_PI;
        }

        Textures convexHullTexture(mapSize, mapSize);
        convexHullTexture.drawPoints(clusters[i], {255, 0, 0});
        for (const Point<int>& point : clusters[i])
        {
            vector<double> weights = convexHull.computeMVC(point);
            Point<int> targetPoint = {0, 0};
            for (uint i = 0; i < contourSize; ++i)
            {
                if (isnan(weights[i]))
                    break;
                int x = targetPoint.getX() + circleHull[i].getX() * weights[i];
                int y = targetPoint.getY() + circleHull[i].getY() * weights[i];
                targetPoint = Point<int>(x, y);
            }
            textures.drawPoint(point, circle.getNormal(targetPoint));
        }
        textures.drawPoints(contour, {0, 255, 255});
    }

    textures.save(".", "textured_");
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        cerr << "[USAGE] ./lichens path/to/config_file" << endl;
        return 1;
    }

    // Parse config
    string configFilename = argv[1];
    ifstream configFile(configFilename);
    if (not configFile.is_open())
    {
        cerr << "[ERROR] Can not open " << configFilename << endl;
        return 1;
    }


    unordered_map<string, string> configMap;
    string line;
    while (getline(configFile, line))
    {
        if (line[0] == '#')
            continue;
        stringstream lineStream(line);
        string field, value;
        if (lineStream >> field and lineStream >> value) 
        {
            field.pop_back();
            lineStream >> value;
            configMap.insert({field, value});
        }
    }

    const vector<Point<int>>& dlaPoints = runDla(configMap);

    const vector<vector<Point<int>>>& clusters = runKMeans(configMap, dlaPoints);

    textureParametrizaion(configMap, clusters);
}
