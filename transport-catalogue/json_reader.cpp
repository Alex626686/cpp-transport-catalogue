#include "json_reader.h"

#include <sstream>

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

using namespace std::literals;

const json::Node& JsonReader::GetBaseRequests() const{
	const auto& dict = input_.GetRoot().AsDict();
	const auto it = dict.find("base_requests"s);
	if (it == dict.end()) {
		return node_null_;
	}
	return it->second;

}

const json::Node& JsonReader::GetStatRequests() const{
	const auto& dict = input_.GetRoot().AsDict();
	const auto it = dict.find("stat_requests"s);
	if (it == dict.end()) {
		return node_null_;
	}
	return it->second;
}

const json::Node& JsonReader::GetRenderSettings() const{
	const auto& dict = input_.GetRoot().AsDict();
	const auto it = dict.find("render_settings"s);
	if (it == dict.end()) {
		return node_null_;
	}
	return it->second;
}

const json::Node& JsonReader::GetRouterSettings() const{
	const auto& dict = input_.GetRoot().AsDict();
	const auto it = dict.find("routing_settings"s);
	if (it == dict.end()) {
		return node_null_;
	}
	return it->second;
}


void JsonReader::FillCatalogue(tc::TransportCatalogue& cataclogue) const{
	const json::Array arr = GetBaseRequests().AsArray();
	std::vector<std::tuple<std::string_view, std::string_view, int>> vec_dist;

	for (const auto& request : arr) {
		const auto& stops_map = request.AsDict();
		if (stops_map.at("type"s).AsString() == "Stop"sv) {
			auto [name, coordinates, distances] = GetStopVars(stops_map);
			cataclogue.AddStop(name, coordinates);
			for (const auto& [name2, dist] : distances) {
				vec_dist.push_back(std::make_tuple( name, name2, dist));
			}
		}
	}

	for (const auto& [stop_from, stop_to, dist] : vec_dist) {
		cataclogue.SetDistanceStop(stop_from, stop_to, dist);
	}

	for (const auto& request : arr) {
		const auto& buses_map = request.AsDict();
		if (buses_map.at("type"s).AsString() == "Bus"sv) {
			auto [bus_name, stops, is_roundtrip] = GetBusVars(buses_map);
			cataclogue.AddBus(bus_name, stops, is_roundtrip);
		}
	}
}



svg::Color FillColor(const json::Node& node) {
	if (node.IsString()) {
		return node.AsString();
	}
	const auto& arr = node.AsArray();
	uint8_t red = arr[0].AsInt();
	uint8_t green = arr[1].AsInt();
	uint8_t blue = arr[2].AsInt();
	if (arr.size() > 3) {
		return svg::Rgba{ red, green, blue, arr[3].AsDouble() };
	}
	return svg::Rgb{ red, green, blue };
}

render::RenderSettings JsonReader::FillRenderSettings() const{
	json::Dict dict = GetRenderSettings().AsDict();
	render::RenderSettings settings;
	
	settings.width = dict.at("width"s).AsDouble();
	settings.height = dict.at("height"s).AsDouble();
	settings.padding = dict.at("padding"s).AsDouble();
	settings.stop_radius = dict.at("stop_radius"s).AsDouble();
	settings.line_width = dict.at("line_width"s).AsDouble();

	settings.bus_label_font_size = dict.at("bus_label_font_size"s).AsInt();
	const auto& bus_label_offset = dict.at("bus_label_offset"s).AsArray();
	settings.bus_label_offset = {bus_label_offset[0].AsDouble(), bus_label_offset[1].AsDouble()};

	settings.stop_label_font_size = dict.at("stop_label_font_size"s).AsInt();
	const auto& stop_label_offset = dict.at("stop_label_offset"s).AsArray();
	settings.stop_label_offset = { stop_label_offset[0].AsDouble(), stop_label_offset[1].AsDouble()};

	settings.underlayer_color = FillColor(dict.at("underlayer_color"s));
	settings.underlayer_width = dict.at("underlayer_width"s).AsDouble();

	for (const json::Node& color_node : dict.at("color_palette"s).AsArray()) {
		settings.color_palette.push_back(FillColor(color_node));
	}
	return settings;
}

RouterSettings JsonReader::FillRouterSettings() const{
	const json::Node& settings = GetRouterSettings();
	return RouterSettings{ settings.AsDict().at("bus_wait_time"s).AsInt(), settings.AsDict().at("bus_velocity"s).AsDouble() };
}

void JsonReader::Print(const RequestHandler& handler) const {
	const json::Array arr = GetStatRequests().AsArray();
	json::Array result;

	for (const auto& request : arr) {
		const auto& map_request = request.AsDict();
		const auto& type = map_request.at("type"s).AsString();
		if (type == "Stop"s) {
			result.push_back(PrintStop(map_request, handler).AsDict());
		}
		if (type == "Bus"s) {
			result.push_back(PrintBus(map_request, handler).AsDict());
		}
		if (type == "Map"s) {
			result.push_back(PrintMap(map_request, handler).AsDict());
		}
		if (type == "Route"s) {
			result.push_back(PrintRoute(map_request, handler).AsDict());
		}
	}

	json::Print(json::Document{ result }, std::cout);
}

json::Node JsonReader::PrintStop(const json::Dict& map_request, const RequestHandler& handler) const{
	json::Node result;
	const std::string_view name = map_request.at("name"s).AsString();
	const int id = map_request.at("id"s).AsInt();
	const tc::TransportCatalogue& catalogue = handler.GetCatalogue();

	if (catalogue.GetStop(name)) {
		json::Array buses;
		for (const auto& bus : catalogue.GetBusesOnStop(name)) {
			buses.push_back(std::string(bus));
		}

		result = json::Builder().StartDict().
			Key("request_id"s).Value(id).
			Key("buses"s).Value(buses).
			EndDict().Build();
	}
	else {
		result = json::Builder().StartDict().
			Key("request_id"s).Value(id).
			Key("error_message"s).Value("not found"s).
			EndDict().Build();
	}
	return result;
}

json::Node JsonReader::PrintBus(const json::Dict& map_request, const RequestHandler& handler) const{
	json::Node result;
	const std::string_view name = map_request.at("name"s).AsString();
	const int id = map_request.at("id"s).AsInt();
	const tc::TransportCatalogue& catalogue = handler.GetCatalogue();

	if (const tc::Bus* bus = catalogue.GetBus(name)) {
		const tc::BusStats& bus_stats = catalogue.GetBusStats(*bus);

		result = json::Builder().StartDict().
			Key("request_id"s).Value(id).
			Key("curvature"s).Value(bus_stats.curvature).
			Key("route_length"s).Value(bus_stats.route_length).
			Key("stop_count"s).Value(bus_stats.stops_on_route).
			Key("unique_stop_count"s).Value(bus_stats.unique_stops).
			EndDict().Build();
	}
	else {
		
		result = json::Builder().StartDict().
			Key("request_id"s).Value(id).
			Key("error_message"s).Value("not found"s).
			EndDict().Build();
	}
	return result;
}

json::Node JsonReader::PrintMap(const json::Dict& map_request, const RequestHandler& handler) const{
	std::ostringstream osstrm;
	render::MapRenderer mr(FillRenderSettings());
	const tc::TransportCatalogue& catalogue = handler.GetCatalogue();

	svg::Document doc = mr.GetSvg(catalogue.GetAllBusses());
	doc.Render(osstrm);

	json::Node result = json::Builder().StartDict().
		Key("request_id"s).Value(map_request.at("id"s).AsInt()).
		Key("map"s).Value(osstrm.str()).
		EndDict().Build();
	return result;
}

json::Node JsonReader::PrintRoute(const json::Dict& map_request, const RequestHandler& handler) const {
	json::Node result;
	const std::string_view from = map_request.at("from"s).AsString();
	const std::string_view to = map_request.at("to"s).AsString();
 	const int id = map_request.at("id"s).AsInt();	
	const auto& route = handler.GetRouter().FindRoute(from, to);

	if (!route) {
		result = json::Builder{}
			.StartDict()
			.Key("request_id"s).Value(id)
			.Key("error_message"s).Value("not found"s)
			.EndDict()
			.Build();
	}
	else {
		json::Array items;
		double total_time = 0.0;
		items.reserve(route.value().edges.size());
		for (auto& edge_id : route.value().edges) {
			const graph::Edge<double> edge = handler.GetRouter().GetEdge(edge_id);
			if (edge.count_move != 0) {
				items.emplace_back(json::Node(json::Builder{}
					.StartDict()
					.Key("bus"s).Value(edge.name)
					.Key("span_count"s).Value(static_cast<int>( edge.count_move))
					.Key("time"s).Value(edge.weight)
					.Key("type"s).Value("Bus"s)
					.EndDict()
					.Build()));

				total_time += edge.weight;
				}
			else {
				items.emplace_back(json::Node(json::Builder{}
					.StartDict()
					.Key("stop_name"s).Value(edge.name)
					.Key("time"s).Value(edge.weight)
					.Key("type"s).Value("Wait"s)
					.EndDict()
					.Build()));

				total_time += edge.weight;
			}
		}
		result = json::Builder{}
			.StartDict()
			.Key("request_id"s).Value(id)
			.Key("total_time"s).Value(total_time)
			.Key("items"s).Value(items)
			.EndDict()
			.Build();
	}

	return result;
	
}

std::tuple<std::string_view, geo::Coordinates, std::map<std::string_view, int>> JsonReader::GetStopVars(const json::Dict& request_map) const{
	std::string_view name = request_map.at("name"s).AsString();
	geo::Coordinates coordinates { request_map.at("latitude"s).AsDouble(), request_map.at("longitude").AsDouble() };

	std::map<std::string_view, int> stop_distances;
	auto& distances = request_map.at("road_distances"s).AsDict();
	for (auto& [stop_name, dist] : distances) {
		stop_distances.emplace(stop_name, dist.AsInt());
	}

	return std::make_tuple(name, coordinates, stop_distances);
}

std::tuple<std::string_view, std::vector<std::string_view>, bool> JsonReader::GetBusVars(const json::Dict& request_map) const{
	std::string_view name = request_map.at("name"s).AsString();
	std::vector<std::string_view> stops;
	bool is_roundtrip = request_map.at("is_roundtrip").AsBool();
	for (auto& stop : request_map.at("stops"s).AsArray()) {
		stops.push_back(stop.AsString());
	}
	return { name, stops ,is_roundtrip };
}





