#include "dla.hh"
#include "textures.hh"
#include "contour.hh"
#include "point.hh"
#include "kmeans.hh"

#include <iostream>
#include <sstream>
#include <unordered_map>

string dtostr(double value)
{
    ostringstream oss;
    oss << setprecision(8) << noshowpoint << value;
    return oss.str();
}

string getField(const unordered_map<string, string> &configMap, string key)
{
    unordered_map<string, string>::const_iterator it = configMap.find(key);
    if (it != configMap.end())
        return it->second;
    cerr << "[ERROR] " << key << " not found in the config file" << endl;
    return "";
}

vector<Point<int>> runDla(unordered_map<string, string> &configMap)
{
    string centersFilename = getField(configMap, "centers_file");
    int radius = stoi(getField(configMap, "radius"));

    vector<uint8_t> color = {128, 128, 128};
    Textures textures;
    // Run dla if centers file has not been specified
    if (centersFilename == "-")
    {
        int points = stoi(getField(configMap, "points"));
        double alpha = stod(getField(configMap, "alpha"));
        double sigma = stod(getField(configMap, "sigma"));
        double tau = stod(getField(configMap, "tau"));
        double p = stod(getField(configMap, "p"));

        // Run dla
        DLA dla(radius, alpha, sigma, tau, p);
        dla.run(points);
        cout << endl;

        vector<Point<double>> dlaPoints = dla.getPoints();
        vector<Point<int>> dlaPointsInt = vector<Point<int>>(dlaPoints.begin(), dlaPoints.end());

        // Save centers as integers with 0,0 as minimum coordinate
        ofstream outFile("centers_file.txt");
        vector<Point<double>> boundingBox = dla.getBoundingBox();
        int height = abs(boundingBox[1].getX() - boundingBox[0].getX()) + radius * 2 + 2;
        int width = abs(boundingBox[1].getY() - boundingBox[0].getY()) + radius * 2 + 2;
        // Two first fields are height and width
        outFile << height << " " << width << endl;
        for (Point<int> &point : dlaPointsInt)
        {
            point += {abs(boundingBox[0].getX()) + radius + 1, abs(boundingBox[0].getY()) + radius + 1};
            outFile << point.getX() << " " << point.getY() << endl;
        }

        textures = Textures(width, height);
        textures.drawCircles(dlaPointsInt, color, radius);
    }
    else
        textures.readSphereCenterPoints(centersFilename, color, radius);
    configMap.insert({"height", to_string(textures.getHeight())});
    configMap.insert({"width", to_string(textures.getWidth())});
    return textures.getDrawnPoints();
}

vector<vector<Point<int>>> runKMeans(const unordered_map<string, string> &configMap, const vector<Point<int>> &drawnPoints)
{
    int maxIt = stoi(getField(configMap, "max_it"));
    int k = stoi(getField(configMap, "k"));
    int height = stoi(getField(configMap, "height"));
    int width = stoi(getField(configMap, "width"));
    int minClusterSize = stoi(getField(configMap, "min_cluster_size"));
    double minCircularity = stod(getField(configMap, "min_circularity"));
    string imgDir = getField(configMap, "img_dir");

    // Run first division
    KMeans kmeans(drawnPoints, k);
    kmeans.run(maxIt);

    vector<vector<Point<int>>> clusters = kmeans.getClusters();
    vector<Point<int>> centroids = kmeans.getCentroids();
    vector<double> clusterRatios(k);
    for (uint i = 0; i < k; ++i)
        clusterRatios[i] = kmeans.clusterCircleRatio(i);
    vector<bool> modifiedCluster(k, false);

    // Save first division
    Textures textures(width, height);
    for (const vector<Point<int>> &cluster : clusters)
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
            uint it = 1;
            uint newK = 2;
            uint maxK = clusterSize / minClusterSize;
            while (maxK >= 2 and not isCircle)
            {
                // Increment subdivision every 5 iterations
                if (it % 5 == 0)
                    ++newK;

                KMeans newKmeans = KMeans(clusters[j], newK);
                newKmeans.run(maxIt, minCircularity);

                isCircle = true;
                for (uint l = 0; l < newK and isCircle; ++l)
                    isCircle = isCircle and (newKmeans.clusterCircleRatio(l) >= minCircularity);
                isCircle = isCircle or (newK == maxK);

                // If clusters are circular enough, save subdivision
                if (isCircle)
                {
                    vector<vector<Point<int>>> newClusters = newKmeans.getClusters();
                    vector<Point<int>> newCentroids = newKmeans.getCentroids();
                    centroids[j] = newCentroids[0];
                    clusters[j] = newClusters[0];
                    clusters.insert(clusters.end(), newClusters.begin() + 1, newClusters.end());
                    centroids.insert(centroids.end(), newCentroids.begin() + 1, newCentroids.end());
                    cout << "[KMeans] Final clusters circularity:" << endl;
                    for (uint l = 0; l < newK; ++l)
                        cout << "[KMeans] Cluster " << l << " ratio: " << newKmeans.clusterCircleRatio(l) << endl;
                    cout << endl;
                }

                ++it;
            }
        }
    }

    // Save final subdivision
    for (uint i = 0; i < clusters.size(); ++i)
    {
        textures.drawPoints(clusters[i], {uint8_t(rand() / double(RAND_MAX) * 255),
                                          uint8_t(rand() / double(RAND_MAX) * 255),
                                          uint8_t(rand() / double(RAND_MAX) * 255)});
    }

    textures.save(imgDir, "final_");

    return clusters;
}

void textureParametrizaion(const unordered_map<string, string> &configMap, const vector<vector<Point<int>>> &clusters)
{
    int imgWidth = stoi(getField(configMap, "width"));
    int imgHeight = stoi(getField(configMap, "height"));
    string imgDir = getField(configMap, "img_dir");

    Textures circle(1000, 1000);
    Point<int> center = {500, 500};
    double radius = 400;
    circle.drawCircle(center, {255, 0, 0}, radius, true);

    // Load textures
    string textureNamePrefix = getField(configMap, "texture_name_prefix");
    Image lichen;
    vector<vector<vector<uint8_t>>> lichenColorMap = lichen.load(textureNamePrefix + "color_map.png", 3);
    vector<vector<vector<uint8_t>>> lichenOpacityMap = lichen.load(textureNamePrefix + "opacity_map.png", 1);
    vector<vector<vector<uint8_t>>> lichenNormalMap = lichen.load(textureNamePrefix + "normal_map.png", 3);
    vector<vector<vector<uint8_t>>> lichenDisplacementMap = lichen.load(textureNamePrefix + "displacement_map.png", 3);
    int height = lichenColorMap.size();
    int width = lichenColorMap[0].size();

    cout << endl;
    cout << "[Param] Loaded textures" << endl;

    Textures lichenTexture(height, width);
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
            if (lichenOpacityMap[i][j][0] != 0)
                lichenTexture.drawPoint({i, j}, lichenColorMap[i][j], lichenNormalMap[i][j], lichenDisplacementMap[i][j]);
    }
    // Compute texture convex hull
    Contour lichenConvexHull(lichenTexture.getDrawnPoints());
    lichenConvexHull.computeConvexHull();
    vector<Point<int>> lichenContour = lichenConvexHull.getContour();

    // Compute circle proxy hull based on texture hull
    uint contourSize = lichenContour.size();
    vector<Point<int>> circleHull = vector<Point<int>>(contourSize);
    double currentAngle = 0;
    double perimeter = lichenConvexHull.getPerimeter();
    for (uint i = 0; i < contourSize; ++i)
    {
        int x = cos(currentAngle) * radius + center.getX();
        int y = sin(currentAngle) * radius + center.getY();
        circleHull[i] = Point<int>(x, y);
        currentAngle += (lichenConvexHull.getSideLength(i) / perimeter) * 2 * M_PI;
    }

    Contour circleConvexHull;
    circleConvexHull.setContour(circleHull);
    vector<Point<int>> circleDrawnPoints = circle.getDrawnPoints();
    for (const Point<int> &point : circleDrawnPoints)
    {
        vector<double> weights = circleConvexHull.computeMVC(point);
        Point<int> targetPoint = {0, 0};
        for (uint i = 0; i < contourSize; ++i)
        {
            int x = targetPoint.getX() + lichenContour[i].getX() * weights[i];
            int y = targetPoint.getY() + lichenContour[i].getY() * weights[i];
            targetPoint = Point<int>(x, y);
        }

        circle.drawPoint(point,
                         lichenTexture.getColor(targetPoint),
                         lichenTexture.getNormal(targetPoint),
                         lichenTexture.getDisplacement(targetPoint));
    }
    circle.save(imgDir, "circle_proxy_");

    cout << "[Param] Created circle proxy map" << endl;

    int scaleRatio = stoi(getField(configMap, "scale_ratio"));
    Textures textures(imgWidth * scaleRatio, imgHeight * scaleRatio);
    for (uint i = 0; i < clusters.size(); ++i)
    {
        if (clusters[i].size() == 0)
            continue;

        // Upscale clusters if needed
        vector<Point<int>> scaledCluster;
        if (scaleRatio == 1)
            scaledCluster = clusters[i];
        else
        {
            for (const Point<int> &point : clusters[i])
            {
                Point<int> basePoint = point * scaleRatio;
                for (uint i = 0; i < scaleRatio; ++i)
                {
                    for (uint j = 0; j < scaleRatio; ++j)
                    {
                        Point<int> currPoint = basePoint + Point<int>(i, j);
                        scaledCluster.push_back(currPoint);
                    }
                }
            }
        }

        Contour convexHull(scaledCluster);
        convexHull.computeConvexHull();
        vector<Point<int>> contour = convexHull.getContour();

        // Compute the circle hull using the proportions of the cluster hull
        uint contourSize = contour.size();
        vector<Point<int>> circleHull = vector<Point<int>>(contourSize);
        perimeter = convexHull.getPerimeter();
        double currentAngle = 0;
        for (uint j = 0; j < contourSize; ++j)
        {
            int x = cos(currentAngle) * radius + center.getX();
            int y = sin(currentAngle) * radius + center.getY();
            circleHull[j] = Point<int>(x, y);
            currentAngle += (convexHull.getSideLength(j) / perimeter) * 2 * M_PI;
        }

        // Translate cluster coordinates to circle coordinates
        for (Point<int> point : scaledCluster)
        {
            vector<double> weights = convexHull.computeMVC(point);
            Point<int> targetPoint = {0, 0};
            for (uint j = 0; j < contourSize; ++j)
            {
                int x = targetPoint.getX() + circleHull[j].getX() * weights[j];
                int y = targetPoint.getY() + circleHull[j].getY() * weights[j];
                targetPoint = Point<int>(x, y);
            }

            textures.drawPoint(point,
                                circle.getColor(targetPoint),
                                circle.getNormal(targetPoint),
                                circle.getDisplacement(targetPoint));
        }

    }

    textures.save(imgDir, "textured_");
}

int main(int argc, char **argv)
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

    const vector<Point<int>> &dlaPoints = runDla(configMap);

    const vector<vector<Point<int>>> &clusters = runKMeans(configMap, dlaPoints);

    textureParametrizaion(configMap, clusters);
}
