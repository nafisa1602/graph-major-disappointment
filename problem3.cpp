#include <bits/stdc++.h>
using namespace std;
#include "graph.hpp"
#include "kml_generator.hpp"  
struct DijkstraResult 
{
    vector<double> cost;
    vector<int> parent;
    vector<int> parentMode;  
};
DijkstraResult dijkstraCost(int source)
{
    int n = graph.size();
    vector<double> cost(n, 1e18);
    vector<int> parent(n, -1); 
    vector<int> parentMode(n, -1);  
    priority_queue<pair<double,int>, vector<pair<double,int>>, greater<pair<double,int>>> pq;
    cost[source] = 0;
    pq.push({0, source});
    while(!pq.empty())
    {
        pair<double,int> top = pq.top();
        pq.pop();
        double curCost = top.first;
        int u = top.second;
        if(curCost > cost[u]) continue;
        for(auto &edge : graph[u])
        {
            double newCost = curCost + edge.distance * modeCost[edge.mode];
            if(newCost < cost[edge.to])
            {
                cost[edge.to] = newCost;
                parent[edge.to] = u;  
                parentMode[edge.to] = edge.mode;  
                pq.push({newCost, edge.to});
            }
        }
    }
    return {cost, parent, parentMode};  
}

int main()
{
    readCsv("Roadmap-Dhaka.csv", true, CAR);
    readCsv("Routemap-DhakaMetroRail.csv", false, METRO);
    readCsv("Routemap-BikolpoBus.csv", false, BIKOLPOBUS);
    readCsv("Routemap-UttaraBus.csv", false, UTTARABUS);
    double slon, slat, dlon, dlat;
    cin >> slon >> slat >> dlon >> dlat;
    int s = findNearestNode(slon, slat);
    int d = findNearestNode(dlon, dlat);
    auto result = dijkstraCost(s);  
    if(result.cost[d] == 1e18) 
    {
        cout << "No route available\n";
    } 
    else 
    {
        cout << "Cheapest cost (All Transport): " << result.cost[d] << " Taka\n";
        vector<int> path;
        int curr = d;
        while(curr != -1) 
        {
            path.push_back(curr);
            curr = result.parent[curr];
        }
        reverse(path.begin(), path.end());
        vector<int> modes;
        curr = d;
        while(result.parent[curr] != -1) 
        {
            modes.push_back(result.parentMode[curr]);
            curr = result.parent[curr];
        }
        reverse(modes.begin(), modes.end());
        vector<pair<double,double>> coords;
        for(int nodeId : path) 
        {
            coords.push_back(coordinate[nodeId]);
        }
        vector<string> modeNames;
        for(int mode : modes) 
        {
            if(mode == CAR) modeNames.push_back("Car");
            else if(mode == METRO) modeNames.push_back("Metro");
            else if(mode == BIKOLPOBUS) modeNames.push_back("BikolpoBus");
            else if(mode == UTTARABUS) modeNames.push_back("UttaraBus");
            else if(mode == WALK) modeNames.push_back("Walk");
        }
        writeAdvancedKML(
            "problem3_route.kml",
            coords,
            modeNames,
            {slon, slat},
            {dlon, dlat},
            "Problem 3: Cheapest Route (All Transport)",
            "Cost: " + to_string(result.cost[d]) + " Taka"
        );
        cout << "\nKML file created: problem3_route.kml" << endl;
        cout << "Upload to https://www.google.com/mymaps to view!" << endl;
    }
}