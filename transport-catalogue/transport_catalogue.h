#pragma once
#include <vector>
#include <unordered_set>
#include <string_view>
#include <unordered_map>
#include "geo.h"
#include <deque>
#include <string>
#include <set>

struct Stop {
	std::string name;
	Coordinates cords;
};

struct Bus {
	std::string name;
	std::vector<Stop*> stops;
};

class TransportCatalogue {	
	std::deque<Stop> stops_;
	std::unordered_map<std::string_view, Stop*> stops_map_;

	std::deque<Bus> buses_;
	std::unordered_map<std::string_view, Bus*> buses_map_;
	std::unordered_map<std::string_view, std::set<std::string_view>> buses_on_stop_;
	Bus dummy{};
public:
	void AddStop(std::string& name, Coordinates p);
	void AddBus(std::string& name, std::vector<std::string_view> stops);
	const Bus& GetBus(std::string_view name) const;
	bool CheckStop(std::string_view stop) const;
	const std::vector<std::string_view> GetBusesOnStop(const std::string_view& name) const;

};