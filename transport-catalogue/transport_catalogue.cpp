#include "transport_catalogue.h"
#include <string>

void TransportCatalogue::AddStop(std::string& name, Coordinates p) {
	stops_.push_back({ std::move(name), std::move(p) });
	stops_map_[stops_.back().name] = &stops_.back();
}
void TransportCatalogue::AddBus(std::string& name, std::vector<std::string_view> stops) {
	std::vector<Stop*> vec;
	buses_.push_back({ std::move(name), std::move(vec) });
	for (std::string_view str : stops) {
		//vec.push_back(std::move(stops_map_.at(str)));
		buses_.back().stops.push_back(std::move(stops_map_.at(str)));
		buses_on_stop_[stops_map_.at(str)->name].insert(buses_.back().name);
		buses_map_[buses_.back().name] = &buses_.back();
	}
}

const Bus& TransportCatalogue::GetBus(std::string_view name) const {
	if (buses_map_.count(name)) {
		return *buses_map_.at(name);
	}
	return dummy;
}

bool TransportCatalogue::CheckStop(std::string_view stop) const{
	return stops_map_.count(stop);
}

const std::vector<std::string_view> TransportCatalogue::GetBusesOnStop(const std::string_view& name) const{
	std::vector<std::string_view> res;
	if (buses_on_stop_.count(name)) {
		for (const std::string_view& str : buses_on_stop_.at(name)) {
			res.push_back(str);
		}
	}
	return res;
}

