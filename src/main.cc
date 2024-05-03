#include "dla.hh"
#include "textures.hh"
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

vector<Point<int>> runDla(const unordered_map<string, string>& configMap)
{
    int mapSize = stoi(configMap.find("img_size")->second);
    int points = stoi(configMap.find("points")->second);
    int radius = stoi(configMap.find("radius")->second);
    double alpha = stod(configMap.find("alpha")->second);
    double sigma = stod(configMap.find("sigma")->second);
    double tau = stod(configMap.find("tau")->second);
    double p = stod(configMap.find("p")->second);
    string centersFilename = configMap.find("centers_file")->second;

    Textures textures(mapSize, mapSize);
    vector<uint8_t> color = {128, 128, 128};
    if (centersFilename == "-")
    {
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

void runKMeans(const unordered_map<string, string>& configMap, const vector<Point<int>>& drawnPoints)
{
    int maxIt = stoi(configMap.find("max_it")->second);
    int k = stoi(configMap.find("k")->second);
    int mapSize = stoi(configMap.find("img_size")->second);
    int minClusterSize = stoi(configMap.find("min_cluster_size")->second);

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
    textures.save("img", "first_");

    for (uint j = 0; j < k; ++j)
    {
        uint clusterSize = clusters[j].size();
        if (clusterSize > 1)
        {
            double clusterRatio = clusterRatios[j];
            bool isCircle = clusterRatio >= 0.8;
            modifiedCluster[j] = true;
            int it = 1;
            int newK = 2;
            int maxK = clusterSize / minClusterSize;
            while (maxK >= 2 and not isCircle)
            {
                if (it % 10 == 0)
                    ++newK;
                KMeans newKmeans = KMeans(clusters[j], newK);
                newKmeans.run(maxIt);
                isCircle = true;
                for (uint l = 0; l < newK and isCircle; ++l)
                    isCircle = isCircle and (newKmeans.clusterCircleRatio(l) >= 0.8);
                isCircle = isCircle or (newK == maxK);
                if (isCircle) 
                {
                    vector<vector<Point<int>>> newClusters = newKmeans.getClusters();
                    vector<Point<int>> newCentroids = newKmeans.getCentroids();
                    centroids[j] = newCentroids[0];
                    clusters.insert(clusters.end(), newClusters.begin(), newClusters.end());
                    centroids.insert(centroids.end(), newCentroids.begin(), newCentroids.end());
                }
                ++it;
            }
        }
        cout << j << endl;
    }

    textures.clear();
    for (uint i = 0; i < clusters.size(); ++i)
    {
        textures.drawPoints(clusters[i], {uint8_t(rand() / double(RAND_MAX) * 255),
                                          uint8_t(rand() / double(RAND_MAX) * 255),
                                          uint8_t(rand() / double(RAND_MAX) * 255)});
    }
    textures.drawPoints(centroids, {255, 0, 0});
    textures.save("img", "final_");
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

    runKMeans(configMap, dlaPoints);
}
