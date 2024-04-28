#include "stat_reader.h"

void PrintBus(const Bus& bus, std::ostream& output) {

    output << "Bus " << bus.name << ": " << bus.stops.size() << " stops on route, " << CountUniqueStops(bus) << " unique stops, " << ComputeRoute(bus) << " route length" << std::endl;
}

void PrintStop(const std::vector<std::string_view>& buses, std::ostream& output) {

    if (buses.empty()) {
        output << "no buses" << std::endl;
        return;
    }
    
    output << "buses ";
    for (const std::string_view& str : buses) {
        output << str << ' ';
    }
    output << std::endl;
}

void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request,
                       std::ostream& output) {
    auto space = request.find_first_of(' ');
    std::string_view id = request.substr(0, space);
    std::string_view name = request.substr(space + 1, request.size() - space);
    if (id == "Bus") {
        const Bus& bus = transport_catalogue.GetBus(name);
        if (bus.name.size() == 0) {
            output << request << ": not found" << std::endl;
            return;
        }
        PrintBus(bus, output);
    }
    else if (id == "Stop") {
        output << "Stop " << name << ": ";
        if (!transport_catalogue.CheckStop(name)) {
            output << "not found" << std::endl;
            return;
        }
        auto buses = transport_catalogue.GetBusesOnStop(name);
        PrintStop(buses, output);

    }
}

int CountUniqueStops(const Bus& bus){
    std::set<std::string_view> set;
    for (const Stop* s : bus.stops) {
        set.insert(s->name);
    }
    return set.size();
}

inline double ComputeRoute(const Bus& bus){
    double res = 0;
    for (int i = 0; i < bus.stops.size() - 1; ++i) {
        res += ComputeDistance(bus.stops[i]->cords, bus.stops[i + 1]->cords);
    }
    return res;
}
