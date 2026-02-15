#include "graph.hpp"
#include "kml_generator.hpp"

int toMinutes(int hour, int minute)
{
    return hour * 60 + minute;
}
int nextDeparture(int currentTime, int mode)
{
    if(mode == CAR) return currentTime; // Car is always available
    
    int start = 6 * 60;   // 6:00 AM
    int end   = 23 * 60;  // 11:00 PM
    int interval = 15;    // Every 15 minutes
    
    if(currentTime > end) return -1; // After 11 PM, no service
    if(currentTime <= start) return start; // Before 6 AM, wait until 6 AM
    int difference = currentTime - start;
    int nextTime = start + ((difference + interval - 1) / interval) * interval;
    if(nextTime > end) return -1;
    return nextTime;
}
struct State
{
    int time;  
    int node;
    int currentTime; 
    
    bool operator>(const State& other) const
    {
        return time > other.time;
    }
};
struct PathInfo
{
    int totalTime;
    int arrivalTime;
    vector<int> path;
    vector<int> modes;
    vector<int> times; 
    bool found;
};
PathInfo findFastestPath(int source, int destination, int startTime)
{
    PathInfo result;
    result.found = false;
    result.totalTime = INT_MAX;
    int n = graph.size();
    vector<int> bestTime(n, INT_MAX);
    vector<int> parent(n, -1);
    vector<int> parentMode(n, -1);
    vector<int> arrivalTime(n, -1);
    priority_queue<State, vector<State>, greater<State>> pq;
    bestTime[source] = 0;
    arrivalTime[source] = startTime;
    pq.push({0, source, startTime});
    while(!pq.empty())
    {
        State current = pq.top();
        pq.pop();
        int u = current.node;
        int elapsedTime = current.time;
        int currentTimeOfDay = current.currentTime;
        if(elapsedTime > bestTime[u]) continue;
        if(u == destination)
        {
            result.found = true;
            result.totalTime = elapsedTime;
            result.arrivalTime = currentTimeOfDay;
            break; 
        }
        for(auto &edge : graph[u])
        {
            int v = edge.to;
            int mode = edge.mode;
            double dist = edge.distance;
            int departTime = nextDeparture(currentTimeOfDay, mode);
            if(departTime == -1) continue;
            int waitTime = departTime - currentTimeOfDay;
            double travelTimeHours = dist / 10.0;
            int travelTimeMinutes = (int)ceil(travelTimeHours * 60);
            int newArrivalTime = departTime + travelTimeMinutes;
            int newElapsedTime = elapsedTime + waitTime + travelTimeMinutes;
            if(newElapsedTime < bestTime[v])
            {
                bestTime[v] = newElapsedTime;
                arrivalTime[v] = newArrivalTime;
                parent[v] = u;
                parentMode[v] = mode;
                pq.push({newElapsedTime, v, newArrivalTime});
            }
        }
    }
    if(result.found)
    {
        vector<int> path, modes;
        int current = destination;
        while(current != -1)
        {
            path.push_back(current);
            if(parent[current] != -1)
            {
                modes.push_back(parentMode[current]);
            }
            current = parent[current];
        }
        reverse(path.begin(), path.end());
        reverse(modes.begin(), modes.end());
        vector<int> times;
        times.push_back(startTime);
        int curTime = startTime;
        for(int i = 0; i < modes.size(); i++)
        {
            int u = path[i];
            int v = path[i+1];
            int mode = modes[i];
            double dist = 0;
            for(auto &edge : graph[u])
            {
                if(edge.to == v && edge.mode == mode)
                {
                    dist = edge.distance;
                    break;
                }
            }
            int departTime = nextDeparture(curTime, mode);
            double travelTimeHours = dist / 10.0;
            int travelTimeMinutes = (int)ceil(travelTimeHours * 60);
            curTime = departTime + travelTimeMinutes;
            times.push_back(curTime);
        }
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
    cout << "    Problem 5: Fastest Route (Time)    " << endl;
    cout << "=======================================" << endl;
    cout << "Total nodes loaded: " << coordinate.size() << endl << endl;
    double slon, slat, dlon, dlat;
    int startHour, startMinute;
    cout << "Enter source (longitude latitude): ";
    cin >> slon >> slat;
    cout << "Enter destination (longitude latitude): ";
    cin >> dlon >> dlat;
    cout << "Enter start time (hour minute in 24h format, e.g., 17 43): ";
    cin >> startHour >> startMinute;
    int s = findNearestNode(slon, slat);
    int d = findNearestNode(dlon, dlat);
    int startTime = toMinutes(startHour, startMinute);
    double walkToStart = haversine(slon, slat, coordinate[s].first, coordinate[s].second);
    double walkFromEnd = haversine(coordinate[d].first, coordinate[d].second, dlon, dlat);
    int walkStartTime = (int)ceil((walkToStart / 2.0) * 60); 
    int walkEndTime = (int)ceil((walkFromEnd / 2.0) * 60);
    cout << "\n=======================================" << endl;
    cout << "Starting Journey at " << minutesToTime(startTime) << endl;
    cout << "=======================================" << endl;
    if(walkToStart > 0.01)
    {
        cout << "\n1. Walk to nearest network point:" << endl;
        cout << "   Distance: " << fixed << setprecision(3) << walkToStart << " km" << endl;
        cout << "   Time: " << walkStartTime << " minutes (at 2 km/h)" << endl;
        cout << "   Cost: FREE" << endl;
    }
    int networkStartTime = startTime + walkStartTime;
    PathInfo result = findFastestPath(s, d, networkStartTime);
    if(!result.found)
    {
        cout << "\nNo route found!" << endl;
        return 0;
    }
    cout << "\n2. Network Travel:" << endl;
    cout << "   Start: " << minutesToTime(networkStartTime) << endl;
    cout << "   End: " << minutesToTime(result.arrivalTime) << endl;
    cout << "   Duration: " << result.totalTime << " minutes" << endl;
    cout << "   Number of segments: " << result.modes.size() << endl;
    if(walkFromEnd > 0.01)
    {
        cout << "\n3. Walk to final destination:" << endl;
        cout << "   Distance: " << fixed << setprecision(3) << walkFromEnd << " km" << endl;
        cout << "   Time: " << walkEndTime << " minutes (at 2 km/h)" << endl;
        cout << "   Cost: FREE" << endl;
    }
    int finalArrival = result.arrivalTime + walkEndTime;
    int totalDuration = finalArrival - startTime;
    cout << "\n=======================================" << endl;
    cout << "       SUMMARY" << endl;
    cout << "=======================================" << endl;
    cout << "Departure Time: " << minutesToTime(startTime) << endl;
    cout << "Arrival Time: " << minutesToTime(finalArrival) << endl;
    cout << "Total Duration: " << totalDuration << " minutes (" 
         << (totalDuration / 60) << "h " << (totalDuration % 60) << "m)" << endl;
    double totalCost = 0;
    for(int i = 0; i < result.path.size() - 1; i++)
    {
        int u = result.path[i];
        int v = result.path[i+1];
        int mode = result.modes[i];
        
        for(auto &edge : graph[u])
        {
            if(edge.to == v && edge.mode == mode)
            {
                totalCost += edge.distance * modeCost[mode];
                break;
            }
        }
    }
    cout << "Total Cost: " << fixed << setprecision(2) << totalCost << " Taka (informational)" << endl;
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
        int waitTime = 0;
        int departTime = nextDeparture(time, mode);
        if(mode != CAR)
        {
            waitTime = departTime - time;
        }
        int travelTime = nextTime - departTime;
        cout << "\nStep " << stepNum++ << ":" << endl;
        cout << "  Mode: " << modeToString(mode) << endl;
        cout << "  From: (" << fixed << setprecision(4) << coordinate[u].first 
             << ", " << coordinate[u].second << ")" << endl;
        cout << "  To: (" << coordinate[v].first << ", " << coordinate[v].second << ")" << endl;
        if(waitTime > 0)
        {
            cout << "  Wait: " << waitTime << " minutes" << endl;
        }
        cout << "  Depart: " << minutesToTime(departTime) << endl;
        cout << "  Arrive: " << minutesToTime(nextTime) << endl;
        cout << "  Travel Time: " << travelTime << " minutes" << endl;
        cout << "  Distance: " << setprecision(3) << dist << " km" << endl;
        cout << "  Cost: " << setprecision(2) << (dist * modeCost[mode]) << " Taka" << endl;
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
                    "Duration: " + to_string(totalDuration) + " minutes";
    writeAdvancedKML(
        "problem5_route.kml",
        pathCoords,
        modeNames,
        {slon, slat},
        {dlon, dlat},
        "Problem 5: Fastest Route",
        summary
    );
    cout << "\n✓ SUCCESS! KML file created: problem5_route.kml" << endl;
    cout << "\nTo view on Google My Maps:" << endl;
    cout << "1. Go to https://www.google.com/mymaps" << endl;
    cout << "2. Click 'CREATE A NEW MAP'" << endl;
    cout << "3. Click 'Import' in the left panel" << endl;
    cout << "4. Upload problem5_route.kml" << endl;
    cout << "5. Your fastest route will appear with colored segments!" << endl;
    return 0;
}