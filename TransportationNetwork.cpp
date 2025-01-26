#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <queue>
#include <stack>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include "TransportNetwork.h"

int TransportNetwork::Stop::id = 0;

void TransportNetwork::displayStopsWithIds()
{
    for (const auto &[id, stop] : graph)
    {
        std::cout << "Id: " << id << ", " << "stop name: " << stop.name << std::endl;
    }
}

void TransportNetwork::displayStopSchedule(const int &stopId)
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

void TransportNetwork::displayBusSchedule(const int &busNumber)
{
    std::cout << "The schedule of bus " << busNumber << " is: " << std::endl;
    for (int i = 0; i < busRoutes[busNumber].size(); i++)
    {
        int st = busRoutes[busNumber][i];
        std::cout << graph[st].name << " : ";
        if (i == 0 && st == *(busRoutes[busNumber].end() - 1))
        {
            for (int j = 0; j < graph[st].buses[busNumber].size() / 2; j++)
            {
                std::cout << graph[st].buses[busNumber][j] << " ";
            }
        }
        else if (i == busRoutes[busNumber].size() - 1 && st == busRoutes[busNumber][0])
        {
            for (int j = graph[st].buses[busNumber].size() / 2; j < graph[st].buses[busNumber].size(); j++)
            {
                std::cout << graph[st].buses[busNumber][j] << " ";
            }
        }
        else
        {
            for (int t : graph[st].buses[busNumber])
            {
                std::cout << t << " ";
            }
        }
        std::cout << std::endl;
    }
}

void TransportNetwork::displayStopBusSchedule(const int &stopId, const int &busNumber)
{
    std::cout << "Bus " << busNumber << " in on stop " << graph[stopId].name << " at: " << std::endl;
    for (int time : graph[stopId].buses[busNumber])
    {
        std::cout << time << " ";
    }
    std::cout << std::endl;
}

std::vector<int> lineToVetcor(const std::string &line)
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

    return result;
}

void TransportNetwork::deserialize(const std::string &fileName)
{
    std ::ifstream stream(fileName);
    if (!stream)
    {
        throw std::runtime_error("Error with the opening of the file");
    }
    std::string line;
    while (std::getline(stream, line) && line != "#")
    {
        addStop(line);
    }

    int busNumber;

    std::vector<int> route;
    std::vector<std::vector<int>> timeTable;
    while (!stream.eof())
    {
        stream >> busNumber;
        std::getline(stream, line);
        std::getline(stream, line);
        route = lineToVetcor(line);
        for (int i = 0; i < route.size(); ++i)
        {
            std::getline(stream, line);
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
        addBus(busNumber, route, timeTable);
        timeTable.clear();
    }
    stream.close();
}

void TransportNetwork::serialize(const std::string &fileName)
{
    std::ofstream stream(fileName, std::ios::trunc);
    if (!stream)
    {
        throw std::runtime_error("Error with the opening of the file");
    }
    std::stack<Stop> invertedStop;
    for (const auto &pair : graph)
    {
        invertedStop.push(pair.second);
    }
    while (!invertedStop.empty())
    {
        stream << invertedStop.top().name << "\n";
        invertedStop.pop();
    }
    stream << "#\n";
    std::stack<std::pair<int, std::vector<int>>> invertedBuses;
    for (const auto &pair : busRoutes)
    {
        invertedBuses.push(pair);
    }
    while (!invertedBuses.empty())
    {
        std::pair<int, std::vector<int>> pair = invertedBuses.top();
        invertedBuses.pop();
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
        if (invertedBuses.empty())
        {
            stream << "$";
        }
        else
        {
            stream << "$\n";
        }
    }
}

void TransportNetwork::addStop(const std::string &name)
{

    Stop temp;
    temp.name = name;
    graph[temp.id++] = temp;
}

void TransportNetwork::addBus(int number, const std::vector<int> &route, const std::vector<std::vector<int>> &timetable)
{
    if (busRoutes.find(number) != busRoutes.end())
    {
        throw std::runtime_error("Error: There is already bus ");
    }
    if (route.size() != timetable.size())
    {
        throw std::runtime_error("Error: Route size and timetable size do not match for bus ");
    }
    busRoutes[number] = route;

    for (int i = 0; i < route.size(); i++)
    {
        if (graph.find(route[i]) == graph.end())
        {
            throw std::runtime_error("Error: No such stop");
        }

        graph[route[i]].buses[number].insert(graph[route[i]].buses[number].end(), timetable[i].begin(), timetable[i].end());
    }
}

void TransportNetwork::removeStop(const std::string &name)
{
    auto stopIt = std::find_if(graph.begin(), graph.end(),
                               [&name](const auto &pair)
                               {
                                   return pair.second.name == name;
                               });

    if (stopIt == graph.end())
    {
        std::cout << "Stop \"" << name << "\" not found." << std::endl;
        return;
    }

    int stopId = stopIt->first;

    for (auto &[busId, route] : busRoutes)
    {
        route.erase(std::remove(route.begin(), route.end(), stopId), route.end());
    }

    graph.erase(stopId);

    std::cout << "Stop \"" << name << "\" removed successfully." << std::endl;
}

void TransportNetwork::removeBus(int number)
{
    auto busIt = busRoutes.find(number);
    if (busIt == busRoutes.end())
    {
        std::cout << "Bus " << number << " not found." << std::endl;
        return;
    }

    for (auto &[stopId, stop] : graph)
    {
        stop.buses.erase(number);
    }

    busRoutes.erase(busIt);

    std::cout << "Bus " << number << " removed successfully." << std::endl;
}

void TransportNetwork::minTimeRoute(const int &startStop, const int &endStop, const int &startTime)
{
    std::vector<Triple> result = getMinTimeRoute(startStop, endStop, startTime);
    Triple temp = {-1, -1, -1};
    if (result[0] == temp)
    {
        std::cout << "There is no route from \"" << graph[startStop].name << "\" to \"" << graph[endStop].name << "\" at " << startTime << std::endl;
        return;
    }

    std::cout << "Fastest route from \"" << graph[startStop].name << "\" to \"" << graph[endStop].name << "\" at " << startTime << " is:" << std::endl;

    for (auto [stop, bus, time] : result)
    {
        std::cout << "(" << graph[stop].name << ", " << bus << ", " << time << ") ";
    }
    std::cout << std::endl;
}

void TransportNetwork::minChangesRoute(const int &startStop, const int &endStop, const int &startTime)
{
    std::vector<Triple> path = getMinTimeRoute(startStop, endStop, startTime);
    Triple temp = {-1, -1, -1};
    if (path[0] == temp)
    {
        std::cout << "There is no route from \"" << graph[startStop].name << "\" to \"" << graph[endStop].name << "\" at " << startTime << std::endl;
        return;
    }
    std::vector<Triple> result = getMinChangesRoute(startStop, endStop, startTime);
    std::cout << "The route with minimum bus changes from \"" << graph[startStop].name << "\" to \"" << graph[endStop].name << "\" at " << startTime << " is:" << std::endl;
    for (auto [stop, bus, time] : result)
    {
        std::cout << "(" << graph[stop].name << ", " << bus << ", " << time << ") ";
    }
    std::cout << std::endl;
}

void TransportNetwork::minWaitRoute(const int &startStop, const int &endStop, const int &startTime)
{
    std::vector<Triple> path = getMinTimeRoute(startStop, endStop, startTime);
    Triple temp = {-1, -1, -1};
    if (path[0] == temp)
    {
        std::cout << "There is no route from \"" << graph[startStop].name << "\" to \"" << graph[endStop].name << "\" at " << startTime << std::endl;
        return;
    }

    std::vector<Triple> result = getMinWaitRoute(startStop, endStop, startTime);
    std::cout << "The route with minimum waiting outside from \"" << graph[startStop].name << "\" to \"" << graph[endStop].name << "\" at " << startTime << " is:" << std::endl;

    for (auto [stop, bus, time] : result)
    {
        std::cout << "(" << graph[stop].name << ", " << bus << ", " << time << ") ";
    }
    std::cout << std::endl;
}

std::vector<TransportNetwork::Triple> TransportNetwork::getMinTimeRoute(const int &startStop, const int &endStop, const int &startTime)
{
    if (startStop == endStop)
        return {{startStop, -1, startTime}};

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
        for (const auto &[busNumber, arrivalTimes] : graph[minId].buses)
        {
            int indexRoute = std::find(busRoutes[busNumber].begin(), busRoutes[busNumber].end(), minId) - busRoutes[busNumber].begin();
            int indexTime = 0;

            int size = arrivalTimes.size();
            if (indexRoute == 0 && busRoutes[busNumber][0] == *(busRoutes[busNumber].end() - 1))
            {
                size = size / 2;
            }

            while (indexTime < size && arrivalTimes[indexTime] < minTime[minId])
            {
                ++indexTime;
            }

            if (indexRoute + 1 >= busRoutes[busNumber].size() || indexTime >= size)
            {
                continue;
            }
            int toStop = busRoutes[busNumber][indexRoute + 1];

            int time = graph[toStop].buses[busNumber][indexTime];

            if (minTime[toStop] > time)
            {
                minTime[toStop] = time;
                prev[toStop] = {minId, busNumber, arrivalTimes[indexTime]};
            }
        }
    }

    std::vector<Triple> restoredPath;

    for (int curr = endStop; curr != startStop; curr = prev[curr].stop)
    {
        if (minTime[curr] < prev[curr].departureTime)
        {
            return {{-1, -1, -1}};
        }
        if (curr == endStop)
        {
            restoredPath.push_back({curr, -1, minTime[curr]});
        }
        restoredPath.push_back(prev[curr]);
    }
    std::reverse(restoredPath.begin(), restoredPath.end());
    return restoredPath;
}

std::vector<TransportNetwork::Triple> TransportNetwork::getMinChangesRoute(const int &startStop, const int &endStop, const int &startTime)
{
    if (startStop == endStop)
        return {{startStop, -1, startTime}};

    std::queue<Triple> q;
    std::unordered_map<int, bool> visitedStops;
    std::unordered_map<int, std::unordered_map<int, bool>> visitedBuses;
    std::unordered_map<int, Triple> prev;

    for (const auto &[id, _] : graph)
    {
        visitedStops[id] = false;
        prev[id] = {-1, -1, -1};
    }

    q.push({startStop, -1, startTime});
    visitedStops[startStop] = true;
    prev[startStop] = {-1, -1, startTime};

    while (!q.empty())
    {
        Triple curr = q.front();
        q.pop();

        int currStop = curr.stop;
        int currTime = curr.departureTime;

        if (currStop == endStop)
            break;

        for (const auto &[busNumber, arrivalTimes] : graph[currStop].buses)
        {
            if (visitedBuses[currStop][busNumber])
                continue;

            visitedBuses[currStop][busNumber] = true;

            auto &route = busRoutes[busNumber];
            auto it = std::find(route.begin(), route.end(), currStop);
            if (it == route.end() - 1)
                continue;

            int routeIndex = it - route.begin();

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
                continue;

            auto itt = std::find(graph[currStop].buses[busNumber].begin(), graph[currStop].buses[busNumber].end(), departureTime);
            int timeIndex = itt - graph[currStop].buses[busNumber].begin();

            for (int i = routeIndex + 1; i < route.size(); i++)
            {
                int nextStop = route[i];
                if (visitedStops[nextStop])
                    continue;

                q.push({nextStop, busNumber, graph[nextStop].buses[busNumber][timeIndex]});
                visitedStops[nextStop] = true;

                prev[nextStop] = {currStop, busNumber, departureTime};

                if (nextStop == endStop)
                    break;
            }
        }
    }

    if (!visitedStops[endStop])
        return {{-1, -1, -1}};

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

std::vector<TransportNetwork::Triple> TransportNetwork::getMinWaitRoute(const int &startStop, const int &endStop, const int &startTime)
{
    std::unordered_map<int, std::unordered_map<int, int>> minWaitTime;
    std::unordered_map<int, std::unordered_map<int, Triple>> prev;
    std::unordered_map<int, std::unordered_map<int, bool>> visited;

    int firstTime = 1440;
    int numOfNodes = 0;

    for (const auto &[id, stop] : graph)
    {
        for (const auto &[busNumber, arrivalTimes] : stop.buses)
        {
            for (int time : arrivalTimes)
            {
                minWaitTime[id][time] = INT_MAX / 2;
                visited[id][time] = false;
                numOfNodes++;
                if (id == startStop && time < firstTime && time >= startTime)
                {
                    firstTime = time;
                }
            }
        }
    }

    minWaitTime[startStop][firstTime] = firstTime - startTime;
    prev[startStop][firstTime] = {startStop, -1, startTime};

    for (int i = 0; i < numOfNodes; ++i)
    {
        int minEdgeId = -1;
        int minEdgeTime = -1;

        // Find the next unvisited node with the minimum wait time
        for (const auto &[id, times] : minWaitTime)
        {
            for (const auto &[time, wait] : times)
            {
                if (!visited[id][time] && (minEdgeId == -1 || wait < minWaitTime[minEdgeId][minEdgeTime]))
                {
                    minEdgeId = id;
                    minEdgeTime = time;
                }
            }
        }

        if (minEdgeId == -1 || minEdgeTime == -1 || minWaitTime[minEdgeId][minEdgeTime] == INT_MAX / 2)
        {
            break;
        }

        visited[minEdgeId][minEdgeTime] = true;

        for (const auto &[busNumber, arrivalTimes] : graph[minEdgeId].buses)
        {
            int indexRoute = std::find(busRoutes[busNumber].begin(), busRoutes[busNumber].end(), minEdgeId) - busRoutes[busNumber].begin();

            // Handle waiting edges
            for (int time : arrivalTimes)
            {
                if (time > minEdgeTime)
                {
                    int val = minWaitTime[minEdgeId][minEdgeTime] + (time - minEdgeTime);
                    if (val < minWaitTime[minEdgeId][time])
                    {
                        minWaitTime[minEdgeId][time] = val;
                        prev[minEdgeId][time] = {minEdgeId, -1, minEdgeTime};
                    }
                }
            }

            if (indexRoute + 1 >= busRoutes[busNumber].size())
            {
                continue;
            }

            // Handle traveling edges
            int indexTime = std::find(arrivalTimes.begin(), arrivalTimes.end(), minEdgeTime) - arrivalTimes.begin();
            if (indexTime == arrivalTimes.size())
            {
                continue;
            }

            int toStop = busRoutes[busNumber][indexRoute + 1];
            std::vector<int> arrivalT = graph[toStop].buses[busNumber];
            int diff = graph[toStop].buses[busNumber].size() - graph[minEdgeId].buses[busNumber].size();
            int toTime = (indexRoute + 2 == busRoutes[busNumber].size())
                             ? arrivalT[indexTime + diff]
                             : arrivalT[indexTime];

            if (!visited[toStop][toTime])
            {
                int val = minWaitTime[minEdgeId][minEdgeTime];
                if (val < minWaitTime[toStop][toTime])
                {
                    minWaitTime[toStop][toTime] = val;
                    prev[toStop][toTime] = {minEdgeId, busNumber, minEdgeTime};
                }
            }
        }
    }

    std::vector<Triple> route;
    int currentStop = endStop;
    int currentTime = -1;

    for (const auto &[time, _] : minWaitTime[endStop])
    {
        if (currentTime == -1 || minWaitTime[endStop][time] < minWaitTime[endStop][currentTime])
        {
            currentTime = time;
        }
    }

    if (currentTime == -1)
    {
        return {{-1, -1, -1}};
    }

    while (currentStop != startStop || currentTime != startTime)
    {
        const Triple &prevTriple = prev[currentStop][currentTime];
        route.push_back({currentStop, prevTriple.departureBus, currentTime});
        currentStop = prevTriple.stop;
        currentTime = prevTriple.departureTime;
    }

    route.push_back({startStop, -1, startTime});
    std::reverse(route.begin(), route.end());
    return route;
}

int main()
{
    /*
        TransportNetwork SofiaBusNetwork;
        SofiaBusNetwork.deserialize("informationFile.txt");
        SofiaBusNetwork.minTimeRoute(0, 1, 308);
        SofiaBusNetwork.serialize("test_graph.txt");
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
    network.addBus(94, {0, 1}, {{300, 360}, {310, 370}});
    network.addBus(210, {0, 2, 1, 3, 4, 5, 6, 7, 0}, {{361}, {365}, {369}, {376}, {384}, {400}, {410}, {414}, {430}});
    network.addBus(88, {7, 5, 4}, {{350, 450}, {365, 460}, {380, 470}});

    network.minTimeRoute(0, 1, 360);
    std::cout << std::endl;
    network.minChangesRoute(7, 6, 308);
    std::cout << std::endl;
    network.minTimeRoute(7, 6, 410);
    network.minChangesRoute(7, 6, 410);
    network.minWaitRoute(7, 6, 410);
    std::cout << std::endl;

    TransportNetwork network1;
    network1.addStop("Mladost 1");
    network1.addStop("Mladost 2");
    network1.addStop("Mladost 3");
    network1.addStop("Mladost 4");
    network1.addBus(313, {8, 11, 8}, {{0, 8, 16, 24, 32, 40, 48, 56, 64, 72}, {4, 12, 20, 28, 36, 44, 52, 60, 68, 76}, {8, 16, 24, 32, 40, 48, 56, 64, 72, 80}});
    network1.addBus(122, {8, 9, 10}, {{2, 12, 22, 32, 42, 52}, {10, 20, 30, 40, 50, 60}, {20, 30, 40, 50, 60, 70}});
    network1.minWaitRoute(8, 10, 0);
    std::cout << std::endl;
    network1.minTimeRoute(8, 10, 1);
    std::cout << std::endl;
    network1.removeBus(313);
    std::cout << std::endl;
    network1.minWaitRoute(8, 10, 0);
    std::cout << std::endl;
    network1.removeStop("Mladost 2");
    std::cout << std::endl;
    network1.minWaitRoute(8, 10, 0);

    return 0;
}