#pragma once
#include "geo.h"
#include <string>
#include <vector>

/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки. 
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */

namespace tc{

	struct Stop {
		std::string name;
		geo::Coordinates cords;
	};

	struct Bus {
		std::string name;
		std::vector<Stop*> stops;
		bool is_roundtrip;
	};

	struct BusStats {
		int stops_on_route;
		int unique_stops;
		double route_length;
		double curvature;
	};

}//namespace tc