#include "dla.hh"
#include <iostream>
#include <sstream>
#include <iomanip>

string dtostr(double value)
{
    ostringstream oss;
    oss << setprecision(8) << noshowpoint << value;
    return oss.str();
}

int main(int argc, char** argv)
{
    if (argc < 8)
    {
        cout << "Usage:" << endl;
        cout << "./dla n_points map_size radius alpha sigma tau p" << endl;
        return 1;
    }
    uint nPoints = stoi(argv[1]);
    uint mapSize = stoi(argv[2]);
    double radius = stod(argv[3]);
    double alpha = stod(argv[4]);
    double sigma = stod(argv[5]);
    double tau = stod(argv[6]);
    double p = stod(argv[7]);
    srand(time(0));
    DLA dla(mapSize, radius, alpha, sigma, tau, p);
    string filename = "lichens_m_" + dtostr(mapSize) +
                      "_r_" + dtostr(radius) +
                      "_a_" + dtostr(alpha) +
                      "_s_" + dtostr(sigma) +
                      "_t_" + dtostr(tau) +
                      "_p_" + dtostr(p) + ".txt";

    dla.run(nPoints, filename);
}
