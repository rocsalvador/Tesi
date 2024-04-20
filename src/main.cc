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
    string field;
    while (configFile >> field)
    {
        if (field[0] == '#')
            continue;
        field.pop_back();
        string value;
        configFile >> value;
        configMap.insert({field, value});
    }

    int mapSize = stoi(configMap.find("image_size")->second);
    int points = stoi(configMap.find("points")->second);
    int radius = stoi(configMap.find("radius")->second);
    double alpha = stod(configMap.find("alpha")->second);
    double sigma = stod(configMap.find("sigma")->second);
    double tau = stod(configMap.find("tau")->second);
    double p = stod(configMap.find("p")->second);

    int maxIt = stoi(configMap.find("max_it")->second);
    int k = stoi(configMap.find("k")->second);

    DLA dla(mapSize, radius, alpha, sigma, tau, p);
    dla.run(points);

    cout << endl;

    Textures textures(mapSize, mapSize);
    vector<uint8_t> color = {128, 128, 128};
    vector<Point<double>> dlaPoints = dla.getPoints();
    vector<Point<int>> dlaPointsInt = vector<Point<int>> (dlaPoints.size());
    for (uint i = 0; i < dlaPoints.size(); ++i)
        dlaPointsInt[i] = Point<int>(dlaPoints[i].getX(), dlaPoints[i].getY());
    textures.drawCircles(dlaPointsInt, color, radius);

    KMeans kmeans(textures.getDrawnPoints(), k);
    kmeans.run(maxIt);
    vector<vector<Point<int>>> clusters = kmeans.getClusters();
    for (const vector<Point<int>>& cluster : clusters)
        textures.drawPoints(cluster, {uint8_t(rand() / double(RAND_MAX) * 255),
                                      uint8_t(rand() / double(RAND_MAX) * 255),
                                      uint8_t(rand() / double(RAND_MAX) * 255)});
    textures.save();
}
