#pragma once
#include <stack>
#include <vector>
#include <unordered_map>
#include <string>

class TransportNetwork
{
private:
    struct Stop
    {
        static int id;
        std::string name;
        std::unordered_map<int, std::vector<int>> buses;
    };

    std::unordered_map<int, Stop> graph;                 
    std::unordered_map<int, std::vector<int>> busRoutes; 

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
    void displayStopsWithIds();
    void displayStopSchedule(const int &stopId);
    void displayBusSchedule(const int &busNumber);
    void displayStopBusSchedule(const int &stopId, const int &busNumber);

    void serialize(const std::string &fileName);
    void deserialize(const std ::string &fileName);

    void addStop(const std::string &name);
    void addBus(int number, const std::vector<int> &route, const std::vector<std::vector<int>> &timetable);

    void removeStop(const std::string &name);
    void removeBus(int number);

    void minTimeRoute(const int &startStop, const int &endStop, const int &startTime);
    void minChangesRoute(const int &startStop, const int &endStop, const int &startTime);
    void minWaitRoute(const int &startStop, const int &endStop, const int &startTime);
};
