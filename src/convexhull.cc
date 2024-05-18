#include "convexhull.hh"

ConvexHull::ConvexHull(const vector<Point<int>> &points)
{
    this->points = points;
}

Point<int> ConvexHull::nextToTop(stack<Point<int>> &S)
{
    Point<int> p = S.top();
    S.pop();
    Point<int> res = S.top();
    S.push(p);
    return res;
}

void ConvexHull::swap(Point<int> &p1, Point<int> &p2)
{
    Point<int> temp = p1;
    p1 = p2;
    p2 = temp;
}

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are collinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int ConvexHull::orientation(Point<int> p, Point<int> q, Point<int> r)
{
    int val = (q.getY() - p.getY()) * (r.getX() - q.getX()) -
              (q.getX() - p.getX()) * (r.getY() - q.getY());

    if (val == 0)
        return 0;             // collinear
    return (val > 0) ? 1 : 2; // clock or counterclock wise
}


bool ConvexHull::compare(const Point<int> &p1, const Point<int> &p2)
{
    double o = orientation(p0, p1, p2);
    if (o == 0)
        return Point<int>::sqEuclidianDistance(p0, p2) >= Point<int>::sqEuclidianDistance(p0, p1);

    return o == 2;
}

void ConvexHull::computeConvexHull()
{
    int n = points.size();
    int ymin = points[0].getY(), minimum = 0;
    for (int i = 1; i < n; i++)
    {
        int y = points[i].getY();

        if ((y < ymin) || (ymin == y &&
                           points[i].getX() < points[minimum].getX()))
            ymin = points[i].getY(), minimum = i;
    }

    swap(points[0], points[minimum]);

    p0 = points[0];
    sort(points.begin(), points.end(), [this](const Point<int> &p1, const Point<int> &p2)
         { return compare(p1, p2); });

    int m = 1;
    for (int i = 1; i < n; i++)
    {
        while (i < n - 1 && orientation(p0, points[i],
                                        points[i + 1]) == 0)
            i++;

        points[m] = points[i];
        m++;
    }

    if (m < 3)
        return;

    stack<Point<int>> S;
    S.push(points[0]);
    S.push(points[1]);
    S.push(points[2]);

    for (int i = 3; i < m; i++)
    {
        while (S.size() > 1 && orientation(nextToTop(S), S.top(), points[i]) != 2)
            S.pop();
        S.push(points[i]);
    }

    contour = vector<Point<int>>(S.size());
    uint i = 0;
    minDistance = numeric_limits<double>::max();
    perimeter = 0;
    while (!S.empty())
    {
        Point<int> p = S.top();
        S.pop();
        contour[i] = p;
        if (i != 0)
        {
            double distance = Point<int>::euclidianDistance(contour[i], contour[i - 1]);
            perimeter += distance;
            minDistance = min(distance, minDistance);
        }
        ++i;
    }

    double distance = Point<int>::euclidianDistance(contour[contour.size() - 1], contour[0]);
    perimeter += distance;
    minDistance = min(distance, minDistance);
}

vector<Point<int>> ConvexHull::getContour()
{
    return contour;
}

double ConvexHull::angle(const Point<int> &p1, const Point<int> &p2, const Point<int> &p3)
{
    double ux = p1.getX() - p2.getX();
    double uy = p1.getY() - p2.getY();
    double vx = p3.getX() - p2.getX();
    double vy = p3.getY() - p2.getY();

    double dotProduct = ux * vx + uy * vy;
    double magu = sqrt(ux * ux + uy * uy);
    double magv = sqrt(vx * vx + vy * vy);

    return acos(max(min(dotProduct / (magu * magv), 1.0), -1.0));
}

vector<double> ConvexHull::computeMVC(const Point<int> &point)
{
    int n = contour.size();
    if (n == 0)
        return vector<double> (0);
    vector<double> weights(n, 0);
    vector<double> magnitudes(n);
    vector<double> halfTangents(n);

    magnitudes[0] = Point<int>::euclidianDistance(point, contour[0]);
    for (int i = 0; i < n; ++i)
    {
        int next = (i + 1) % n;
        if (magnitudes[i] < 1)
        {
            weights[i] = 1;
            return weights;
        }

        magnitudes[next] = Point<int>::euclidianDistance(point, contour[next]);
        if (magnitudes[next] < 1)
            return vector<double> (n, 0);

        double a = angle(contour[i], point, contour[next]);
        halfTangents[i] = tan(a * 0.5);
    }

    double totalWeight = 0;
    for (int i = 0; i < n; ++i)
    {
        int prev = (i - 1) % n;
        weights[i] = (halfTangents[prev] + halfTangents[i]) / magnitudes[i];
        totalWeight += weights[i];
    }

    for (double& weight : weights)
        weight /= totalWeight;

    return weights;
}