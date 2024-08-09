#pragma once

#include "domain.h"

#include <deque>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <string_view>
#include <set>
#include <map>





namespace tc {


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
		void AddStop(const std::string_view name, geo::Coordinates coordinates);

		void SetDistanceStop(std::string_view l_stop, std::string_view r_stop, int distance);

		int GetDistanceStop(Stop* stop_from, Stop* stop_to)const;

		void AddBus(const std::string_view& name, const std::vector<std::string_view>& stops, bool is_roundtrip);


		const Bus* GetBus(std::string_view name) const;

		const Stop* GetStop(std::string_view stop) const;

		const std::set<std::string_view> GetBusesOnStop(std::string_view name) const;


		BusStats GetBusStats(const Bus& bus) const;

		//String_view исспользую, для сортировки по алфавиту(условие задания), если убрать надо будет потом сортировать, не знаю, как сделать,
		// чтобы не пострадала эффективность, в map_renderer тоже надо все мапы убрать(еще надо, чтобы остановки не повторялись)?
		std::map<std::string_view, const Bus*> GetAllBusses()const;


	};

}//namespace tc