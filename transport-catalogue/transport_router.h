#pragma once

#include <memory>

#include "router.h"
#include "transport_catalogue.h"

struct RouterSettings {
	int bus_wait_time = 0;
	double bus_velocity = 0;
};

<<<<<<< HEAD
class TransportRouter {
public:
	TransportRouter(const RouterSettings settings, const tc::TransportCatalogue& catalogue)
		: settings_(settings), router_(BuildGraph(catalogue)) {}

	const graph::DirectedWeightedGraph<double> BuildGraph(const tc::TransportCatalogue& catalogue);

	const std::optional<graph::Router<double>::RouteInfo> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;

	const graph::Edge<double> GetEdge(const graph::VertexId& edge_id)const;
=======

class TransportRouter {
public:
	explicit TransportRouter(const RouterSettings settings, const tc::TransportCatalogue& catalogue)
		: settings_(settings){
		BuildGraph(catalogue);
		}

	const std::vector<const graph::Edge<double>*> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;
>>>>>>> d924e7d (2)

private:
	RouterSettings settings_;
	std::map<std::string, graph::VertexId> stops_id_;
<<<<<<< HEAD
	graph::Router<double> router_;

=======
	std::unique_ptr<graph::Router<double>> router_;
	graph::DirectedWeightedGraph<double> graph_;

	void BuildGraph(const tc::TransportCatalogue& catalogue);

	graph::DirectedWeightedGraph<double> FillStopsGraph(const std::map<std::string_view, const tc::Stop*> stops);
	void FillBusesGraph(const tc::TransportCatalogue& catalogue, graph::DirectedWeightedGraph<double>& graph);
>>>>>>> d924e7d (2)
};