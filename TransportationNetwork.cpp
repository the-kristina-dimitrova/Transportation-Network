#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <queue>
#include <fstream>
#include <stdexcept>
#include <algorithm>

class TransportNetwork
{
private:
    struct Stop
    {
        static int id;
        std::string name;
        std::unordered_map<int, std::vector<int>> buses; // Bus number -> arriving time
    };

    std::unordered_map<int, Stop> graph;                 // Stop id ->the actual structure stop
    std::unordered_map<int, std::vector<int>> busRoutes; // Bus number -> bus route;

    struct Triple
    {
        int stop;
        int departureBus;
        int departureTime;
        bool operator==(const Triple &other)
        {
            return stop == other.stop && departureBus == other.departureBus && departureTime == other.departureTime;
        }
    };

    std::vector<Triple> getMinTimeRoute(const int &startStop, const int &endStop, const int &startTime);
    std::vector<Triple> getMinChangesRoute(const int &startStop, const int &endStop, const int &startTime);
    std::vector<Triple> getMinWaitRoute(const int &startStop, const int &endStop, const int &startTime);

public:
    void displayStopsWithIds()
    {
        for (const auto &[id, stop] : graph)
        {
            std::cout << "Id: " << id << ", " << "stop name: " << stop.name << std::endl;
        }
    }
    void displayBusScheduleOfStop(const int &stopId)
    {
        for (const auto &[bus, aTimes] : graph[stopId].buses)
        {
            std::cout << "Bus " << bus << " : ";
            for (int t : aTimes)
            {
                std::cout << t << " ";
            }
            std::cout << std::endl;
        }
    }

    void uploadGraph(const std::string &fileName)
    {
        std::ofstream stream(fileName, std::ios::trunc);
        if (!stream)
        {
            throw std::runtime_error("Error with the opening of the file");
        }
        for (const auto &pair : graph)
        {
            stream << pair.second.name << "\n";
        }
        stream << "#\n";
        for (const auto &pair : busRoutes)
        {
            stream << pair.first << "\n";
            for (int i = 0; i < pair.second.size(); ++i)
            {
                if (i < pair.second.size() - 1)
                {
                    stream << pair.second[i] << " ";
                }
                else
                {
                    stream << pair.second[i] << "\n";
                }
            }
            for (const auto &stop : pair.second)
            {
                std::vector<int> timetable = graph[stop].buses[pair.first];
                for (int i = 0; i < timetable.size(); ++i)
                {
                    if (i < timetable.size() - 1)
                    {
                        stream << timetable[i] << " ";
                    }
                    else
                    {
                        stream << timetable[i] << "\n";
                    }
                }
            }
            stream << "$\n";
        }
    }

    void addStop(const std::string &name)
    {

        Stop temp;
        temp.name = name;
        graph[temp.id++] = temp;
    }

    void addBus(int number, const std::vector<int> &route, const std::vector<std::vector<int>> &timetable)
    {
        if (busRoutes.find(number) != busRoutes.end())
        {
            std::cout << "Error: There is already bus " << number << std::endl;
            return;
        }
        if (route.size() != timetable.size())
        {
            std::cout << "Error: Route size and timetable size do not match for bus " << number << "\n";
            return;
        }
        busRoutes[number] = route;

        for (int i = 0; i < route.size(); i++)
        {
            if (graph.find(route[i]) == graph.end())
            {
                std::cout << "Error: No such stop: " << route[i] << std::endl;
                return;
            }

            graph[route[i]].buses[number].insert(graph[route[i]].buses[number].end(), timetable[i].begin(), timetable[i].end());
            // in this way we manage loop bus routes;
        }
    }

    void minTimeRoute(const int &startStop, const int &endStop, const int &startTime)
    {
        std::vector<Triple> result = getMinTimeRoute(startStop, endStop, startTime);
        for (auto [stop, bus, time] : result)
        {
            std::cout << "(" << stop << ", " << bus << ", " << time << ") ";
        }
    }

    void minChangesRoute(const int &startStop, const int &endStop, const int &startTime)
    {
        std::vector<Triple> result = getMinChangesRoute(startStop, endStop, startTime);
        for (auto [stop, bus, time] : result)
        {
            std::cout << "(" << stop << ", " << bus << ", " << time << ") ";
        }
    }

    void minWaitRoute(const int &startStop, const int &endStop, const int &startTime)
    {
        std::vector<Triple> result = getMinWaitRoute(startStop, endStop, startTime);
        for (auto [stop, bus, time] : result)
        {
            std::cout << "(" << stop << ", " << bus << ", " << time << ") ";
        }
    }
};
int TransportNetwork::Stop::id = 0;

std::vector<TransportNetwork::Triple> TransportNetwork::getMinTimeRoute(const int &startStop, const int &endStop, const int &startTime)
{
    std::unordered_map<int, int> minTime;
    std::unordered_map<int, Triple> prev;
    std::unordered_map<int, bool> visited;
    for (const auto &[id, stop] : graph)
    {
        minTime[id] = INT_MAX / 2;
        visited[id] = false;
    }
    minTime[startStop] = startTime;
    prev[startStop] = {startStop, -1, startTime};
    for (int i = 0; i < graph.size(); i++)
    {
        int minId = -1;

        // this cycle find the next edge with minimum weight(time in this case) and unvisited stop
        for (const auto &[id, time] : minTime)
        {
            if (!visited[id] && (minId == -1 || time < minTime[minId]))
            {
                minId = id;
            }
        }

        if (minTime[minId] == INT_MAX / 2)
            break;

        visited[minId] = true;
        for (auto [busNumber, arrivalTimes] : graph[minId].buses)
        {
            int indexRoute = std::find(busRoutes[busNumber].begin(), busRoutes[busNumber].end(), minId) - busRoutes[busNumber].begin();
            int indexTime;

            int size = arrivalTimes.size();
            if (indexRoute == 0 && busRoutes[busNumber][0] == *(busRoutes[busNumber].end() - 1))
                size = size / 2;
            while (indexTime < size)
            {
                if (arrivalTimes[indexTime] >= minTime[minId])
                {
                    break;
                }
                ++indexTime;
            }

            if (indexRoute + 1 >= busRoutes[busNumber].size())
            {
                continue;
            }
            int toStop = busRoutes[busNumber][indexRoute + 1];

            int time = graph[toStop].buses[busNumber][indexTime];
            if (minTime[toStop] > time)
            {
                minTime[toStop] = time;
                prev[toStop] = {minId, busNumber, graph[minId].buses[busNumber][indexTime]};
            }
        }
    }

    std::vector<Triple> restoredPath;
    for (int curr = endStop; curr != startStop; curr = prev[curr].stop)
    {
        if (curr == endStop)
        {
            restoredPath.push_back({curr, -1, minTime[curr]});
        }
        restoredPath.push_back(prev[curr]);
    }
    std::reverse(restoredPath.begin(), restoredPath.end());
    minTime.clear();
    prev.clear();
    visited.clear();
    return restoredPath;
}

std::vector<TransportNetwork::Triple> TransportNetwork::getMinChangesRoute(const int &startStop, const int &endStop, const int &startTime)
{
    if (startStop == endStop)
        return {{startStop, -1, startTime}};

    // BFS setup
    struct State
    {
        int stop;
        int bus;
        int time;
    };

    std::queue<Triple> q;
    std::unordered_map<int, bool> visitedStops;
    std::unordered_map<int, std::unordered_map<int, bool>> visitedBuses; // stop -> bus -> visited
    std::unordered_map<int, Triple> prev;                                // for reconstructing the path

    for (const auto &[id, _] : graph)
    {
        visitedStops[id] = false;
        prev[id] = {-1, -1, -1};
    }

    // Initialize BFS
    q.push({startStop, -1, startTime});
    visitedStops[startStop] = true;
    prev[startStop] = {-1, -1, startTime};

    while (!q.empty())
    {
        Triple curr = q.front();
        q.pop();

        int currStop = curr.stop;
        int currTime = curr.departureTime;

        // If we've reached the destination stop
        if (currStop == endStop)
            break;

        // Process all buses that stop at the current stop
        for (const auto &[busNumber, arrivalTimes] : graph[currStop].buses)
        {
            // Skip already visited buses at this stop
            if (visitedBuses[currStop][busNumber])
                continue;

            visitedBuses[currStop][busNumber] = true;

            // Find the index of the current stop in the route
            auto &route = busRoutes[busNumber];
            auto it = std::find(route.begin(), route.end(), currStop);
            if (it == route.end() - 1)
                continue;

            int routeIndex = it - route.begin();

            // Find the earliest valid departure time for this bus
            int departureTime = -1;
            if (*(route.begin()) == *(route.end() - 1) && routeIndex == 0)
            {
                for (int i = 0; i < arrivalTimes.size() / 2; i++)
                {
                    int time = arrivalTimes[i];
                    if (time >= currTime)
                    {
                        departureTime = time;
                        break;
                    }
                }
            }
            else
            {
                for (int time : arrivalTimes)
                {
                    if (time >= currTime)
                    {
                        departureTime = time;
                        break;
                    }
                }
            }

            if (departureTime == -1)
                continue; // No valid departure time

            auto itt = std::find(graph[currStop].buses[busNumber].begin(), graph[currStop].buses[busNumber].end(), departureTime);
            int timeIndex = itt - graph[currStop].buses[busNumber].begin();

            // Traverse the route forward from the current stop
            for (int i = routeIndex + 1; i < route.size(); i++)
            {
                int nextStop = route[i];
                if (visitedStops[nextStop])
                    continue;

                // Add the next stop to the BFS queue
                q.push({nextStop, busNumber, graph[nextStop].buses[busNumber][timeIndex]});
                visitedStops[nextStop] = true;

                // Update the previous stop for path reconstruction
                prev[nextStop] = {currStop, busNumber, departureTime};

                // If we've reached the end stop, stop further exploration
                if (nextStop == endStop)
                    break;
            }
        }
    }

    // Check if a path to the end stop was found
    if (!visitedStops[endStop])
        return {{-1, -1, -1}}; // No path found

    // Restore the path from `prev`
    std::vector<Triple> restoredPath;
    for (int curr = endStop; curr != startStop; curr = prev[curr].stop)
    {
        if (curr == endStop)
        {
            std::vector<int> arrivalTimesPrev = graph[prev[curr].stop].buses[prev[curr].departureBus];
            int timeIndex = std::find(arrivalTimesPrev.begin(), arrivalTimesPrev.end(), prev[curr].departureTime) - arrivalTimesPrev.begin();
            restoredPath.push_back({curr, -1, graph[curr].buses[prev[curr].departureBus][timeIndex]});
        }
        restoredPath.push_back(prev[curr]);
    }

    std::reverse(restoredPath.begin(), restoredPath.end());

    return restoredPath;
}

std::vector<TransportNetwork::Triple> TransportNetwork::getMinWaitRoute(const int &startStop, const int &endStop, const int &startTime) {
    // Store the minimum wait time for each stop and time
    std::unordered_map<int, std::unordered_map<int, int>> minWaitTime;
    // Store the previous stop and bus for backtracking
    std::unordered_map<int, std::unordered_map<int, Triple>> prev;
   // Track visited nodes
    std::unordered_map<int, std::unordered_map<int, bool>> visited;

    int firstTime = 1440; // Max time (24 hours in minutes)
    int numOfNodes = 0;

    // Initialize all stops and times
    for (const auto &[id, stop] : graph) {
        for (const auto &[busNumber, arrivalTimes] : stop.buses) {
            for (int time : arrivalTimes) {
                minWaitTime[id][time] = INT_MAX / 2;
                visited[id][time] = false;
                numOfNodes++;
                if (id == startStop && time < firstTime && time >= startTime) {
                    firstTime = time;
                }
            }
        }
    }

    // Initialize the starting stop
    minWaitTime[startStop][firstTime] = firstTime - startTime;
    prev[startStop][firstTime] = {startStop, -1, startTime};

    // Dijkstra's algorithm
    for (int i = 0; i < numOfNodes; ++i) {
        int minEdgeId = -1;
        int minEdgeTime = -1;

        // Find the next unvisited node with the minimum wait time
        for (const auto &[id, times] : minWaitTime) {
            for (const auto &[time, wait] : times) {
                if (!visited[id][time] && (minEdgeId == -1 || wait < minWaitTime[minEdgeId][minEdgeTime])) {
                    minEdgeId = id;
                    minEdgeTime = time;
                }
            }
        }

        if (minEdgeId == -1 || minEdgeTime == -1 || minWaitTime[minEdgeId][minEdgeTime] == INT_MAX / 2) {
            break;
        }

        visited[minEdgeId][minEdgeTime] = true;

        // Debugging: Log visited node
        std::cout << "Visited (" << minEdgeId << ", " << minEdgeTime << ")" << std::endl;

        // Process neighbors (waiting and traveling edges)
        for (const auto &[busNumber, arrivalTimes] : graph[minEdgeId].buses) {
            int indexRoute = std::find(busRoutes[busNumber].begin(), busRoutes[busNumber].end(), minEdgeId) - busRoutes[busNumber].begin();

            // Handle waiting edges
            for (int time : arrivalTimes) {
                if (time > minEdgeTime) {
                    int val = minWaitTime[minEdgeId][minEdgeTime] + (time - minEdgeTime);
                    if (val < minWaitTime[minEdgeId][time]) {
                        minWaitTime[minEdgeId][time] = val;
                        prev[minEdgeId][time] = {minEdgeId, -1, minEdgeTime};
                    }
                }
            }

            // Skip invalid routes
            if (indexRoute + 1 >= busRoutes[busNumber].size()) {
                continue;
            }

            // Handle traveling edges
            int indexTime = std::find(arrivalTimes.begin(), arrivalTimes.end(), minEdgeTime) - arrivalTimes.begin();
            if (indexTime == arrivalTimes.size()) {
                continue;
            }

            int toStop = busRoutes[busNumber][indexRoute + 1];
            std::vector<int> arrivalT = graph[toStop].buses[busNumber];
            int diff = graph[toStop].buses[busNumber].size() - graph[minEdgeId].buses[busNumber].size();
            int toTime = (indexRoute + 2 == busRoutes[busNumber].size())
                         ? arrivalT[indexTime + diff]
                         : arrivalT[indexTime];

            if (!visited[toStop][toTime]) {
                int val = minWaitTime[minEdgeId][minEdgeTime];
                if (val < minWaitTime[toStop][toTime]) {
                    minWaitTime[toStop][toTime] = val;
                    prev[toStop][toTime] = {minEdgeId, busNumber, minEdgeTime};
                }
            }
        }
    }

    // Reconstruct the route
    std::vector<Triple> route;
    int currentStop = endStop;
    int currentTime = -1;

    // Find the earliest arrival time at the end stop
    for (const auto &[time, _] : minWaitTime[endStop]) {
        if (currentTime == -1 || minWaitTime[endStop][time] < minWaitTime[endStop][currentTime]) {
            currentTime = time;
        }
    }
    
    if (currentTime == -1) {
        return {}; // No route found
    }

    // Backtrack using `prev`
    while (currentStop != startStop || currentTime != startTime) {
        const Triple &prevTriple = prev[currentStop][currentTime];
        route.push_back({currentStop, prevTriple.departureBus, currentTime});
        currentStop = prevTriple.stop;
        currentTime = prevTriple.departureTime;
    }

    route.push_back({startStop, -1, startTime});
    std::reverse(route.begin(), route.end());
    return route;
}


std::vector<int> lineToVetcor(const std::string line)
{
    std::vector<int> result;
    int currentNumber = -1;
    for (char c : line)
    {
        if (isdigit(c))
        {
            if (currentNumber == -1)
            {
                currentNumber = (c - '0');
            }
            else
            {
                currentNumber = 10 * currentNumber + (c - '0');
            }
        }
        else if (currentNumber != -1)
        {

            result.push_back(currentNumber);
            currentNumber = -1;
        }
    }
    if (currentNumber != -1)
    {
        result.push_back(currentNumber);
    }

    for (int i : result)
    {
        std::cout << i << " | ";
    }
    std::cout << std::endl;
    return result;
}

void fileRead(TransportNetwork &Network, const std ::string &fileName)
{
    std ::ifstream stream(fileName);
    if (!stream)
    {
        throw std::runtime_error("Error with the opening of the file");
    }
    std::string line;
    while (std::getline(stream, line) && line != "#")
    {
        Network.addStop(line);
    }

    int busNumber;

    std::vector<int> route;
    std::vector<std::vector<int>> timeTable;
    while (!stream.eof())
    {
        stream >> busNumber;
        std::getline(stream, line);
        std::cout << busNumber << std::endl;
        std::getline(stream, line);
        route = lineToVetcor(line);
        for (int i = 0; i < route.size(); ++i)
        {

            if (line == "$")
            {
                throw std::runtime_error("Invalid file - Error Type - :: not enough vectors for timetable");
            }

            timeTable.push_back(lineToVetcor(line));
        }
        std::getline(stream, line);
        if (line != "$")
        {
            throw std::runtime_error("Invalid file - Error Type - :: unexpected input ");
        }
        Network.addBus(busNumber, route, timeTable);
        timeTable.clear();
    }
    stream.close();
}

int main()
{
    /*
        TransportNetwork SofiaBusNetwork;
        fileRead(SofiaBusNetwork, "informationFile.txt");

        SofiaBusNetwork.minTimeRoute(0, 1, 308);
    */

    TransportNetwork network;

    network.addStop("Central station");
    network.addStop("Serdika");
    network.addStop("Vasil Levski");
    network.addStop("Obelq");
    network.addStop("Mladost");
    network.addStop("Sofia university");
    network.addStop("Airport");
    network.addStop("Krasno selo");
    // network.addBus(94, {0, 1}, {{300, 360}, {310, 370}});
    // network.addBus(210, {0, 2, 1, 3, 4, 5, 6, 7, 0}, {{361}, {365}, {369}, {376}, {384}, {400}, {410}, {414}, {430}});
    // network.addBus(88, {7, 5, 4}, {{350, 450}, {365, 460}, {380, 470}});

    // network.minWaitRoute(0, 1, 350);

    TransportNetwork network1;
    network1.addStop("Central station");
    network1.addStop("Serdika");
    network1.addStop("Vasil Levski");
    network1.addStop("Obelq");
    network1.addBus(313, {8, 11, 8}, {{0, 8, 16, 24, 32, 40, 48, 56, 64, 72}, {4, 12, 20, 28, 36, 44, 52, 60, 68, 76}, {8, 16, 24, 32, 40, 48, 56, 64, 72, 80}});
    network1.addBus(122, {8, 9, 10}, {{2, 12, 22, 32, 42, 52}, {10, 20, 30, 40, 50, 60}, {20, 30, 40, 50, 60, 70}});
    network1.minWaitRoute(8, 10, 1);
    // network1.minTimeRoute(11,10,24);
    // network1.displayBusScheduleOfStop(8);

    return 0;
}