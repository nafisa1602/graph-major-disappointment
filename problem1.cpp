#include <bits/stdc++.h>
using namespace std;
#include "graph.hpp"
#include "kml_generator.hpp"
struct DijkstraResult 
{
    vector<double> dist;
    vector<int> parent;
};
DijkstraResult dijkstraDistance(int source)
{
    int n = graph.size();
    vector<double> dist(n, 1e18);
    vector<int> parent(n, -1);  
    priority_queue<pair<double,int>, vector<pair<double,int>>, greater<pair<double,int>>> pq;
    dist[source] = 0;
    pq.push({0, source});
    while(!pq.empty())
    {
        pair<double,int> top = pq.top();
        pq.pop();
        double curDist = top.first;
        int u = top.second;
        if(curDist > dist[u]) continue;
        for(auto &edge : graph[u])
        {
            if(edge.mode != CAR) continue;
            if(curDist + edge.distance < dist[edge.to])
            {
                dist[edge.to] = curDist + edge.distance;
                parent[edge.to] = u;  
                pq.push({dist[edge.to], edge.to});
            }
        }
    }
    return {dist, parent}; 
}
int main()
{
    readCsv("Roadmap-Dhaka.csv", true, CAR);
    double slon, slat, dlon, dlat;
    cin >> slon >> slat >> dlon >> dlat;
    int s = findNearestNode(slon, slat);
    int d = findNearestNode(dlon, dlat);
    auto result = dijkstraDistance(s);  
    if(result.dist[d] == 1e18) 
    {
        cout << "No car route\n";
    } 
    else 
    {
        cout << "Shortest distance: " << result.dist[d] << " km\n";
        vector<int> path;
        int curr = d;
        while(curr != -1) 
        {
            path.push_back(curr);
            curr = result.parent[curr];
        }
        reverse(path.begin(), path.end());
        vector<pair<double,double>> coords;
        for(int nodeId : path) 
        {
            coords.push_back(coordinate[nodeId]);
        }
        writeSimpleKML(
            "problem1_route.kml",
            coords,
            "Problem 1: Shortest Car Route",
            "Distance: " + to_string(result.dist[d]) + " km"
        );
        cout << "\nKML file created: problem1_route.kml" << endl;
        cout << "Upload to https://www.google.com/mymaps to view!" << endl;
    }
}