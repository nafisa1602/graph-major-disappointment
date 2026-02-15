#include "graph.hpp"
#include "kml_generator.hpp"
double modeCostP6[] = {20.0, 5.0, 7.0, 10.0, 0.0};  // Car, Metro, Bikalpa, Uttara, Walk
double modeSpeed[] = {20.0, 15.0, 10.0, 12.0, 2.0};  // Car, Metro, Bikalpa, Uttara, Walk

int toMinutes(int hour, int minute)
{
    return hour * 60 + minute;
}
int nextDeparture(int currentTime, int mode)
{
    if(mode == CAR) return currentTime; // Car is always available
    
    int start, end, interval;
    
    if(mode == METRO)
    {
        start = 1 * 60;    // 1:00 AM
        end = 23 * 60;     // 11:00 PM
        interval = 5;      // Every 5 minutes
    }
    else if(mode == BIKOLPOBUS)
    {
        start = 7 * 60;    // 7:00 AM
        end = 22 * 60;     // 10:00 PM
        interval = 20;     // Every 20 minutes
    }
    else if(mode == UTTARABUS)
    {
        start = 6 * 60;    // 6:00 AM
        end = 23 * 60;     // 11:00 PM
        interval = 10;     // Every 10 minutes
    }
    else
    {
        return -1;
    }
    if(currentTime > end) return -1;
    if(currentTime <= start) return start;
    int difference = currentTime - start;
    int nextTime = start + ((difference + interval - 1) / interval) * interval;
    if(nextTime > end) return -1;
    return nextTime;
}
struct State
{
    double cost;
    int node;
    int currentTime;
    
    bool operator>(const State& other) const
    {
        return cost > other.cost;
    }
};
struct PathInfo
{
    double cost;
    int arrivalTime;
    vector<int> path;
    vector<int> modes;
    vector<int> times;
    bool found;
};
PathInfo findCheapestPathWithDeadline(int source, int destination, int startTime, int deadline)
{
    PathInfo result;
    result.found = false;
    result.cost = 1e18;
    int n = graph.size();
    map<pair<int, int>, double> bestCost;
    map<pair<int, int>, pair<int, int>> parent;  
    map<pair<int, int>, int> parentMode;
    priority_queue<State, vector<State>, greater<State>> pq;
    auto startKey = make_pair(source, startTime);
    bestCost[startKey] = 0;
    pq.push({0, source, startTime});
    double minCost = 1e18;
    int bestArrivalTime = -1;
    while(!pq.empty())
    {
        State current = pq.top();
        pq.pop();
        int u = current.node;
        int t = current.currentTime;
        double c = current.cost;
        auto key = make_pair(u, t);
        if(bestCost.count(key) && bestCost[key] < c) continue;
        if(u == destination && t <= deadline)
        {
            if(c < minCost)
            {
                minCost = c;
                bestArrivalTime = t;
            }
        }
        if(t > deadline) continue;
        for(auto &edge : graph[u])
        {
            int v = edge.to;
            int mode = edge.mode;
            double dist = edge.distance;
            int departTime = nextDeparture(t, mode);
            if(departTime == -1) continue;
            double travelTimeHours = dist / modeSpeed[mode];
            int travelTimeMinutes = (int)ceil(travelTimeHours * 60);
            int arrivalTime = departTime + travelTimeMinutes;
            if(arrivalTime > deadline) continue;
            double edgeCost = dist * modeCostP6[mode];
            double newCost = c + edgeCost;
            auto nextKey = make_pair(v, arrivalTime);
            if(!bestCost.count(nextKey) || newCost < bestCost[nextKey])
            {
                bestCost[nextKey] = newCost;
                parent[nextKey] = key;
                parentMode[nextKey] = mode;
                pq.push({newCost, v, arrivalTime});
            }
        }
    }
    if(minCost < 1e18)
    {
        result.found = true;
        result.cost = minCost;
        result.arrivalTime = bestArrivalTime;
        vector<int> path, modes, times;
        auto current = make_pair(destination, bestArrivalTime);
        while(current.first != -1)
        {
            path.push_back(current.first);
            times.push_back(current.second);
            
            if(parent.count(current))
            {
                modes.push_back(parentMode[current]);
                current = parent[current];
            }
            else
            {
                break;
            }
        }
        reverse(path.begin(), path.end());
        reverse(modes.begin(), modes.end());
        reverse(times.begin(), times.end());
        result.path = path;
        result.modes = modes;
        result.times = times;
    }
    
    return result;
}
string modeToString(int mode)
{
    switch(mode)
    {
        case CAR: return "Car";
        case METRO: return "Metro";
        case BIKOLPOBUS: return "BikolpoBus";
        case UTTARABUS: return "UttaraBus";
        case WALK: return "Walk";
        default: return "Unknown";
    }
}
string minutesToTime(int minutes)
{
    int hour = minutes / 60;
    int min = minutes % 60;
    string period = (hour >= 12) ? "PM" : "AM";
    if(hour > 12) hour -= 12;
    if(hour == 0) hour = 12;
    char buffer[20];
    sprintf(buffer, "%d:%02d %s", hour, min, period.c_str());
    return string(buffer);
}
int main()
{
    readCsv("Roadmap-Dhaka.csv", true, CAR);
    readCsv("Routemap-DhakaMetroRail.csv", false, METRO);
    readCsv("Routemap-BikolpoBus.csv", false, BIKOLPOBUS);
    readCsv("Routemap-UttaraBus.csv", false, UTTARABUS);
    cout << "=======================================" << endl;
    cout << "  Problem 6: Cheapest Route w/ Deadline" << endl;
    cout << "=======================================" << endl;
    cout << "Total nodes loaded: " << coordinate.size() << endl << endl;
    double slon, slat, dlon, dlat;
    int startHour, startMinute, deadlineHour, deadlineMinute;
    cout << "Enter source (longitude latitude): ";
    cin >> slon >> slat;
    cout << "Enter destination (longitude latitude): ";
    cin >> dlon >> dlat;
    cout << "Enter start time (hour minute in 24h format): ";
    cin >> startHour >> startMinute;
    cout << "Enter deadline time (hour minute in 24h format): ";
    cin >> deadlineHour >> deadlineMinute;
    int s = findNearestNode(slon, slat);
    int d = findNearestNode(dlon, dlat);
    int startTime = toMinutes(startHour, startMinute);
    int deadline = toMinutes(deadlineHour, deadlineMinute);
    double walkToStart = haversine(slon, slat, coordinate[s].first, coordinate[s].second);
    double walkFromEnd = haversine(coordinate[d].first, coordinate[d].second, dlon, dlat);
    int walkStartTime = (int)ceil((walkToStart / 2.0) * 60);
    int walkEndTime = (int)ceil((walkFromEnd / 2.0) * 60);
    cout << "\n=======================================" << endl;
    cout << "Starting Journey at " << minutesToTime(startTime) << endl;
    cout << "Must arrive by " << minutesToTime(deadline) << endl;
    cout << "Available time window: " << (deadline - startTime) << " minutes" << endl;
    cout << "=======================================" << endl;
    if(walkToStart > 0.01)
    {
        cout << "\n1. Walk to nearest network point:" << endl;
        cout << "   Distance: " << fixed << setprecision(3) << walkToStart << " km" << endl;
        cout << "   Time: " << walkStartTime << " minutes" << endl;
        cout << "   Cost: FREE" << endl;
    }
    int networkStartTime = startTime + walkStartTime;
    int networkDeadline = deadline - walkEndTime;
    if(networkStartTime >= networkDeadline)
    {
        cout << "\nERROR: Not enough time even for walking!" << endl;
        cout << "   Walking alone takes " << (walkStartTime + walkEndTime) << " minutes" << endl;
        cout << "   But deadline is only " << (deadline - startTime) << " minutes away" << endl;
        return 1;
    }
    cout << "\n2. Network Travel:" << endl;
    cout << "   Must start by: " << minutesToTime(networkStartTime) << endl;
    cout << "   Must finish by: " << minutesToTime(networkDeadline) << endl;
    cout << "   Searching for cheapest route..." << endl;
    PathInfo result = findCheapestPathWithDeadline(s, d, networkStartTime, networkDeadline);
    if(!result.found)
    {
        cout << "\nNo route found that meets the deadline!" << endl;
        cout << "   Try:" << endl;
        cout << "   - Starting earlier" << endl;
        cout << "   - Extending the deadline" << endl;
        cout << "   - Choosing closer locations" << endl;
        return 1;
    }
    cout << "   Found valid route!" << endl;
    cout << "   Start: " << minutesToTime(networkStartTime) << endl;
    cout << "   End: " << minutesToTime(result.arrivalTime) << endl;
    cout << "   Cost: " << fixed << setprecision(2) << result.cost << " Taka" << endl;
    cout << "   Number of segments: " << result.modes.size() << endl;
    if(walkFromEnd > 0.01)
    {
        cout << "\n3. Walk to final destination:" << endl;
        cout << "   Distance: " << fixed << setprecision(3) << walkFromEnd << " km" << endl;
        cout << "   Time: " << walkEndTime << " minutes" << endl;
        cout << "   Cost: FREE" << endl;
    }
    int finalArrival = result.arrivalTime + walkEndTime;
    int slack = deadline - finalArrival;
    cout << "\n=======================================" << endl;
    cout << "       SUMMARY" << endl;
    cout << "=======================================" << endl;
    cout << "Departure Time: " << minutesToTime(startTime) << endl;
    cout << "Arrival Time: " << minutesToTime(finalArrival) << endl;
    cout << "Deadline: " << minutesToTime(deadline) << endl;
    cout << "Slack Time: " << slack << " minutes ";
    if(slack >= 0) cout << "(On time!)" << endl;
    else cout << "(LATE by " << (-slack) << " minutes!)" << endl;
    cout << "Total Duration: " << (finalArrival - startTime) << " minutes" << endl;
    cout << "Total Cost: " << fixed << setprecision(2) << result.cost << " Taka" << endl;
    cout << "\n=======================================" << endl;
    cout << "       COST BREAKDOWN" << endl;
    cout << "=======================================" << endl;
    map<string, double> costByMode;
    map<string, double> distByMode;
    for(int i = 0; i < result.path.size() - 1; i++)
    {
        int u = result.path[i];
        int v = result.path[i+1];
        int mode = result.modes[i];
        for(auto &edge : graph[u])
        {
            if(edge.to == v && edge.mode == mode)
            {
                string modeName = modeToString(mode);
                costByMode[modeName] += edge.distance * modeCostP6[mode];
                distByMode[modeName] += edge.distance;
                break;
            }
        }
    }
    for(auto &p : costByMode)
    {
        cout << p.first << ": " << fixed << setprecision(2) << p.second 
             << " Taka (" << setprecision(3) << distByMode[p.first] << " km)" << endl;
    }
    cout << "\n=======================================" << endl;
    cout << "       DETAILED ROUTE" << endl;
    cout << "=======================================" << endl;
    int stepNum = 1;
    for(int i = 0; i < result.path.size() - 1; i++)
    {
        int u = result.path[i];
        int v = result.path[i+1];
        int mode = result.modes[i];
        int time = result.times[i];
        int nextTime = result.times[i+1];
        double dist = 0;
        for(auto &edge : graph[u])
        {
            if(edge.to == v && edge.mode == mode)
            {
                dist = edge.distance;
                break;
            }
        }
        int departTime = nextDeparture(time, mode);
        int waitTime = (mode == CAR) ? 0 : (departTime - time);
        int travelTime = nextTime - departTime;
        cout << "\nStep " << stepNum++ << ":" << endl;
        cout << "  Mode: " << modeToString(mode) 
             << " (Speed: " << modeSpeed[mode] << " km/h, Cost: ৳" 
             << modeCostP6[mode] << "/km)" << endl;
        
        if(waitTime > 0)
        {
            cout << "  Wait: " << waitTime << " minutes (next departure at " 
                 << minutesToTime(departTime) << ")" << endl;
        }
        cout << "  Depart: " << minutesToTime(departTime) << endl;
        cout << "  Arrive: " << minutesToTime(nextTime) << endl;
        cout << "  Travel Time: " << travelTime << " minutes" << endl;
        cout << "  Distance: " << fixed << setprecision(3) << dist << " km" << endl;
        cout << "  Segment Cost: " << setprecision(2) << (dist * modeCostP6[mode]) << " Taka" << endl;
    }
    cout << "\n=======================================" << endl;
    cout << "\n=======================================" << endl;
    cout << "       GENERATING KML FILE" << endl;
    cout << "=======================================" << endl;
    vector<pair<double,double>> pathCoords;
    if(walkToStart > 0.01) 
    {
        pathCoords.push_back({slon, slat});
    }
    for(int nodeId : result.path) 
    {
        pathCoords.push_back(coordinate[nodeId]);
    }
    if(walkFromEnd > 0.01) 
    {
        pathCoords.push_back({dlon, dlat});
    }
    vector<string> modeNames;
    if(walkToStart > 0.01) 
    {
        modeNames.push_back("Walk");
    }
    for(int mode : result.modes) 
    {
        modeNames.push_back(modeToString(mode));
    }
    if(walkFromEnd > 0.01) 
    {
        modeNames.push_back("Walk");
    }
    string summary = "Start: " + minutesToTime(startTime) + ", " +
                    "Arrival: " + minutesToTime(finalArrival) + ", " +
                    "Deadline: " + minutesToTime(deadline) + ", " +
                    "Cost: " + to_string(result.cost) + " Taka";
    writeAdvancedKML(
        "problem6_route.kml",
        pathCoords,
        modeNames,
        {slon, slat},
        {dlon, dlat},
        "Problem 6: Cheapest Route with Deadline",
        summary
    );
    cout << "\n✓ SUCCESS! KML file created: problem6_route.kml" << endl;
    cout << "\nTo view on Google My Maps:" << endl;
    cout << "1. Go to https://www.google.com/mymaps" << endl;
    cout << "2. Click 'CREATE A NEW MAP'" << endl;
    cout << "3. Click 'Import' in the left panel" << endl;
    cout << "4. Upload problem6_route.kml" << endl;
    cout << "5. Your deadline-constrained route will appear!" << endl;
    return 0;
}