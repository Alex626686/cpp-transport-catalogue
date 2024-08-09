#include "transport_catalogue.h"


namespace tc {

	void TransportCatalogue::AddStop(const std::string_view name, geo::Coordinates coordinates) {
		stops_.push_back({ std::move(name.data()), std::move(coordinates) });
		stops_map_[stops_.back().name] = &stops_.back();
	}

	void TransportCatalogue::SetDistanceStop(std::string_view l_stop, std::string_view r_stop, int distance) {
		Stop* stop_ptr = stops_map_.at(l_stop);
		Stop* stop_ptr2 = stops_map_.at(r_stop); 
		distance_to_stop_[{stop_ptr, stop_ptr2}] = distance;
		if (!distance_to_stop_.count({stop_ptr2, stop_ptr})) {
			distance_to_stop_[{stop_ptr2, stop_ptr}] = distance;
		}
	}

	int TransportCatalogue::GetDistanceStop(Stop* stop_from, Stop* stop_to)const {
		return distance_to_stop_.at({ stop_from,stop_to });
	}



	void TransportCatalogue::AddBus(const std::string_view& name, const std::vector<std::string_view>& stops, bool is_roundtrip) {
		std::vector<Stop*> vec;
		buses_.push_back({ name.data(), std::move(vec), is_roundtrip });
		for (std::string_view str : stops) {
			buses_.back().stops.push_back(std::move(stops_map_.at(str)));
			buses_on_stop_[stops_map_.at(str)->name].insert(buses_.back().name);
			buses_map_[buses_.back().name] = &buses_.back();
		}
	}

	const Bus* TransportCatalogue::GetBus(std::string_view name) const {
		const auto it = buses_map_.find(name);
		if (it != buses_map_.end()) {
			return it->second;
		}
		return nullptr;
	}

	const Stop* TransportCatalogue::GetStop(std::string_view name) const {
		const auto it = stops_map_.find(name);
		if (it != stops_map_.end()) {
			return it->second;
		}
		return nullptr;
	}

	const std::set<std::string_view> TransportCatalogue::GetBusesOnStop(std::string_view name) const {
		const auto it = buses_on_stop_.find(name);
		if (it != buses_on_stop_.end()) {
			return it->second;
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
		std::vector<Stop*> stops = bus.stops;
		if (!bus.is_roundtrip) {
			stops.reserve(stops.size() * 2);
			stops.insert(stops.end(), std::next(stops.rbegin()), stops.rend());
		}
		for (int i = 0; i < stops.size() - 1; ++i) {
			res += ComputeDistance(stops[i]->cords, stops[i + 1]->cords);
		}
		return res;
	}

	double TransportCatalogue::ComputeRealRoute(const Bus& bus) const {
		double real_route = 0;
		std::vector<Stop*> stops = bus.stops;
		if (!bus.is_roundtrip) {
			stops.reserve(stops.size() * 2);
			stops.insert(stops.end(), std::next(stops.rbegin()), stops.rend());
		}
		for (int i = 1; i < stops.size(); ++i) {
			real_route += GetDistanceStop(stops.at(i - 1), stops.at(i));
		}
		return real_route;
	}

	double TransportCatalogue::ComputeCurvature(const double real, const double geo) const {
		return real / geo;
	}

	BusStats TransportCatalogue::GetBusStats(const Bus& bus) const {
		double real_route = ComputeRealRoute(bus);
		double geo_route = ComputeRoute(bus);
		int stops_count = bus.stops.size();
		if (!bus.is_roundtrip) {
			stops_count += bus.stops.size() - 1;
		}
		return { stops_count, CountUniqueStops(bus), real_route, ComputeCurvature(real_route, geo_route) };
	}

	std::map<std::string_view, const Bus*> TransportCatalogue::GetAllBusses() const	{
		std::map<std::string_view, const Bus*> res;
		for (const auto& bus : buses_map_) {
			res.insert(bus);
		}
		return res;
	}

}//namespace tk