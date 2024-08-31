#pragma once

#include <memory>

#include "router.h"
#include "transport_catalogue.h"

struct RouterSettings {
	int bus_wait_time = 0;
	double bus_velocity = 0;
};

class TransportRouter {
public:
	TransportRouter(const RouterSettings settings, const tc::TransportCatalogue& catalogue)
		: settings_(settings), router_(BuildGraph(catalogue)) {}

	const graph::DirectedWeightedGraph<double> BuildGraph(const tc::TransportCatalogue& catalogue);

	const std::optional<graph::Router<double>::RouteInfo> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;

	const graph::Edge<double> GetEdge(const graph::VertexId& edge_id)const;

private:
	RouterSettings settings_;
	std::map<std::string, graph::VertexId> stops_id_;
	graph::Router<double> router_;

};