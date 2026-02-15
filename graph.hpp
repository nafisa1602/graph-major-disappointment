#ifndef GRAPH_HPP
#define GRAPH_HPP
#include <bits/stdc++.h>
using namespace std;
enum Mode { CAR = 0, METRO = 1, BIKOLPOBUS = 2, UTTARABUS = 3, WALK = 4 };
struct Edge
{
    int to;
    double distance;
    int mode;
};
extern vector<vector<Edge>> graph;
extern vector<pair<double,double>> coordinate;
extern double modeCost[];
void readCsv(string fileName, bool isRoadmap, int mode);
int findNearestNode(double longitude, double latitude);
double haversine(double lon1, double lat1, double lon2, double lat2);
#endif
