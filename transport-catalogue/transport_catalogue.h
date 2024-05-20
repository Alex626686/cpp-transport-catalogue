#pragma once
#include <deque>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <string_view>
#include <set>

#include "geo.h"

namespace tc{

struct Stop {
	std::string name;
	Coordinates cords;
};

struct Bus {
	std::string name;
	std::vector<Stop*> stops;
};

struct BusStats {
	size_t stops_on_route;
	int unique_stops;
	double route_length;
	double curvature;
};

class TransportCatalogue {	

	int CountUniqueStops(const Bus& bus) const;

	double ComputeRoute(const Bus& bus) const;

	double ComputeRealRoute(const Bus& bus) const;

	double ComputeCurvature(const double lhs, const double rhs) const;

	std::deque<Stop> stops_;
	std::unordered_map<std::string_view, Stop*> stops_map_;

	std::deque<Bus> buses_;
	std::unordered_map<std::string_view, Bus*> buses_map_;
	std::unordered_map<std::string_view, std::set<std::string_view>> buses_on_stop_;

	class PtrsHasher {
	public:
		size_t operator()(const std::pair<void*, void*> ptrs) const {
			return hasher(ptrs.first) + 8 * hasher(ptrs.second);
		}

	private:
		std::hash<const void*> hasher;
	};

	std::unordered_map<std::pair<Stop*, Stop*>, int, PtrsHasher> distance_to_stop_;


public:
	void AddStop(const std::string& name, Coordinates coordinates);

	void SetDistanceStop(std::string_view l_stop, std::vector<std::pair<std::string_view, int>> r_stop_dist);

	void AddBus(const std::string& name, const std::vector<std::string_view>& stops);

	const Bus* GetBus(std::string_view name) const;

	const Stop* GetStop(std::string_view stop) const;

	const std::set<std::string_view> GetBusesOnStop(std::string_view name) const;

	
	BusStats GetBusStats(const Bus& bus) const;

};

}//namespace tk