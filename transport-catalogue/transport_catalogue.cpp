#include "transport_catalogue.h"

namespace tc{

void TransportCatalogue::AddStop(const std::string& name, Coordinates coordinates) {
	stops_.push_back({ name, std::move(coordinates) });
	stops_map_[stops_.back().name] = &stops_.back();
}


void TransportCatalogue::SetDistanceStop(std::string_view l_stop, std::vector<std::pair<std::string_view, int>> r_stop_dist){
	Stop* stop_ptr = stops_map_.at(l_stop);
	for (const auto& elem : r_stop_dist) {
		Stop* stop_ptr2 = stops_map_.at(elem.first);
		distance_to_stop_[{stop_ptr, stop_ptr2}] = elem.second;
		if (!distance_to_stop_.count({ stop_ptr2, stop_ptr })) {
			distance_to_stop_[{stop_ptr2, stop_ptr}] = elem.second;
		}
	}
}


void TransportCatalogue::AddBus(const std::string& name, const std::vector<std::string_view>& stops) {
	std::vector<Stop*> vec;
	buses_.push_back({ name, std::move(vec) });
	for (std::string_view str : stops) {
		buses_.back().stops.push_back(std::move(stops_map_.at(str)));
		buses_on_stop_[stops_map_.at(str)->name].insert(buses_.back().name);
		buses_map_[buses_.back().name] = &buses_.back();
	}
}

const Bus* TransportCatalogue::GetBus(std::string_view name) const {
	if (buses_map_.count(name)) {
		return buses_map_.at(name);
	}
	return nullptr;
}

const Stop* TransportCatalogue::GetStop(std::string_view name) const{
	if (stops_map_.count(name)) {
		return stops_map_.at(name);
	}
	return nullptr;
}

const std::set<std::string_view> TransportCatalogue::GetBusesOnStop(std::string_view name) const{
	if (buses_on_stop_.count(name)) {
		return buses_on_stop_.at(name);
	}
	return std::set<std::string_view>{};
}

int TransportCatalogue::CountUniqueStops(const Bus& bus) const {
	std::set<std::string_view> set;
	for (const Stop* s : bus.stops) {
		set.insert(s->name);
	}
	return set.size();
}

double TransportCatalogue::ComputeRoute(const Bus& bus) const {
	double res = 0;
	for (int i = 0; i < bus.stops.size() - 1; ++i) {
		res += ComputeDistance(bus.stops[i]->cords, bus.stops[i + 1]->cords);
	}
	return res;
}

double TransportCatalogue::ComputeRealRoute(const Bus& bus) const {
	double real_route = 0;
	for (int i = 1; i < bus.stops.size(); ++i) {
		real_route += distance_to_stop_.at({ bus.stops[i - 1], bus.stops[i]});
	}
	return real_route;
}

double TransportCatalogue::ComputeCurvature(const double real, const double geo) const {
	return real / geo;
}

BusStats TransportCatalogue::GetBusStats(const Bus& bus) const{
	double real_route = ComputeRealRoute(bus);
	double geo_route = ComputeRoute(bus);
	return { bus.stops.size(), CountUniqueStops(bus), real_route, ComputeCurvature(real_route, geo_route)};
}

}//namespace tk