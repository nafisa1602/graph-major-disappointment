#include <bits/stdc++.h>
using namespace std;
#include "graph.hpp"
map<pair<long long, long long>, int> nodeId;
vector<vector<Edge>> graph;
vector<pair<double,double>> coordinate;
double modeCost[] = {20.0, 5.0, 7.0, 7.0, 0.0};
double rad(double degree)
{
    return degree * 3.14159265358979323846 / 180.0;
}
double haversine(double lon1, double lat1, double lon2, double lat2)
{
    const double R = 6371.0;
    lat1 = rad(lat1);
    lat2 = rad(lat2);
    lon1 = rad(lon1);
    lon2 = rad(lon2);
    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;
    double a = sin(dlat/2)*sin(dlat/2) + cos(lat1)*cos(lat2)*sin(dlon/2)*sin(dlon/2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    return R * c;
}
pair<long long,long long> makeKey(double lon, double lat)
{
    return 
    {
        (long long)(lon * 1e6),(long long)(lat * 1e6)
    };
}
int createNode(double longitude, double latitude)
{
    auto key = makeKey(longitude, latitude);
    if(nodeId.count(key)) return nodeId[key];
    int id = coordinate.size();
    nodeId[key] = id;
    coordinate.push_back({longitude, latitude});
    graph.push_back(vector<Edge>());
    return id;
}
vector<double> extractNumber(const string &line)
{
    vector<double> numbers;
    string temp = "";
    for(char c : line)
    {
        if(isdigit(c) || c=='.' || c=='-')
        {
            temp += c;
        }
        else
        {
            if(!temp.empty())
            {
                try
                {
                    numbers.push_back(stod(temp));
                }
                catch(...) 
{
                }
                temp = "";
            }
        }
    }
    if(!temp.empty())
    {
        try 
        {
            numbers.push_back(stod(temp));
        }
        catch(...) {}
    }
    return numbers;
}
void addEdge(int u, int v, double distance, int mode)
{
    graph[u].push_back({v,distance,mode});
    graph[v].push_back({u,distance,mode});
}
void readCsv(string fileName, bool isRoadmap, int mode)
{
    ifstream file(fileName);
    if(!file.is_open())
    {
        cout<<"Cannot open "<<fileName<<endl;
        return;
    }
    string line;
    while(getline(file,line))
    {
        if(line.empty()) continue;
        vector<double> values = extractNumber(line);
        if(values.size()<2) continue;
        int limit = values.size();
        if(isRoadmap && values.size()>=4) limit -= 2;
        vector<int> ids;
        for(int i = 0; i+1 < limit; i+=2)
        {
            int id = createNode(values[i], values[i+1]);
            ids.push_back(id);
        }
        for(int i = 0; i+1 < ids.size(); i++)
        {
            int u=ids[i];
            int v=ids[i+1];
            double dist = haversine(coordinate[u].first, coordinate[u].second, coordinate[v].first, coordinate[v].second);
            addEdge(u,v,dist,mode);
        }
    }
    file.close();
}
int findNearestNode(double longitude, double latitude)
{
    double best=1e18;
    int bestId=-1;
    for(int i = 0; i < coordinate.size(); i++)
    {
        double d=haversine(longitude,latitude, coordinate[i].first, coordinate[i].second);
        if(d<best)
        {
            best=d;
            bestId=i;
        }
    }
    return bestId;
}
