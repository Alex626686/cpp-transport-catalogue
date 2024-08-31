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
	explicit TransportRouter(const RouterSettings settings, const tc::TransportCatalogue& catalogue)
		: settings_(settings){
		BuildGraph(catalogue);
		}

	const std::vector<const graph::Edge<double>*> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;

private:
	RouterSettings settings_;
	std::map<std::string, graph::VertexId> stops_id_;
	std::unique_ptr<graph::Router<double>> router_;
	graph::DirectedWeightedGraph<double> graph_;

	void BuildGraph(const tc::TransportCatalogue& catalogue);

	graph::DirectedWeightedGraph<double> FillStopsGraph(const std::map<std::string_view, const tc::Stop*> stops);
	void FillBusesGraph(const tc::TransportCatalogue& catalogue, graph::DirectedWeightedGraph<double>& graph);
};