#include "stat_reader.h"

void PrintBus(const TransportCatalogue& transport_catalogue, const Bus& bus, std::ostream& output) {
    auto stats = transport_catalogue.GetBusStats(bus);
    output << "Bus " << bus.name
        << ": " << stats.stops_on_route << " stops on route, "
        << stats.unique_stops << " unique stops, "
        << stats.route_length << " route length" << std::endl;
}

void PrintStop(const std::set<std::string_view>& buses, std::ostream& output) {

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
        const Bus* bus = transport_catalogue.GetBus(name);
        if (bus == nullptr) {
            output << request << ": not found" << std::endl;
            return;
        }
        PrintBus(transport_catalogue, *bus, output);
    }
    else if (id == "Stop") {
        output << "Stop " << name << ": ";
        if (!transport_catalogue.GetStop(name)) {
            output << "not found" << std::endl;
            return;
        }
        auto buses = transport_catalogue.GetBusesOnStop(name);
        PrintStop(buses, output);

    }
}

