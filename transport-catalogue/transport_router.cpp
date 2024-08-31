


#include "transport_router.h"

<<<<<<< HEAD
const graph::DirectedWeightedGraph<double> TransportRouter::BuildGraph(const tc::TransportCatalogue& catalogue) {
	const auto& buses = catalogue.GetAllBusses();
	const auto& stops = catalogue.GetAllStops();
	graph::DirectedWeightedGraph<double> graph(stops.size() * 2);

    std::map<std::string, graph::VertexId> stops_id;
    graph::VertexId vertex_id = 0;
=======
void TransportRouter::BuildGraph(const tc::TransportCatalogue& catalogue) {
	graph::DirectedWeightedGraph<double> graph(std::move(FillStopsGraph(catalogue.GetAllStops())));
    FillBusesGraph(catalogue, graph);    
    graph_ = std::move(graph);
    router_ = std::make_unique<graph::Router<double>>(graph_);
}


const std::vector<const graph::Edge<double>*> TransportRouter::FindRoute(const std::string_view stop_from, const std::string_view stop_to) const{
    std::optional<graph::Router<double>::RouteInfo> route_info = router_->BuildRoute(stops_id_.at(stop_from.data()), stops_id_.at(stop_to.data()));
    std::vector<const graph::Edge<double>*> res;
    if (route_info) {
        for (const graph::EdgeId id : route_info->edges) {
            res.push_back(&graph_.GetEdge(id));
        }
    }
    else {
        res.push_back(nullptr);
    }
    return res;
}


graph::DirectedWeightedGraph<double> TransportRouter::FillStopsGraph(const std::map<std::string_view, const tc::Stop*> stops){
    std::map<std::string, graph::VertexId> stops_id;
    graph::VertexId vertex_id = 0;
    graph::DirectedWeightedGraph<double> graph(stops.size() * 2);
>>>>>>> d924e7d (2)

    for (const auto& [stop_name, _] : stops) {
        stops_id[stop_name.data()] = vertex_id;
        graph.AddEdge({
            0,
            stop_name.data(),
            vertex_id,
            ++vertex_id,
            static_cast<double>(settings_.bus_wait_time)
<<<<<<< HEAD
        });
        ++vertex_id;
    }

    stops_id_ = std::move(stops_id);

    std::for_each(
        buses.begin(),
        buses.end(),
        [&graph, this, &catalogue](const auto& item) {
            const auto& bus = item.second;
            const auto& stops = bus->stops;
            size_t stops_count = stops.size();
            for (size_t i = 0; i < stops_count; ++i) {
                for (size_t j = i + 1; j < stops_count; ++j) {
                    const tc::Stop* stop_from = stops[i];
                    const tc::Stop* stop_to = stops[j];
                    int dist_sum = 0;
                    int dist_sum_inverse = 0;
                    for (size_t k = i + 1; k <= j; ++k) {
                        dist_sum += catalogue.GetDistanceStop(stops[k - 1], stops[k]);
                        dist_sum_inverse += catalogue.GetDistanceStop(stops[k], stops[k - 1]);
                    }
                    graph.AddEdge({
                        j - i,
                        bus->name,
                        stops_id_.at(stop_from->name) + 1,
                        stops_id_.at(stop_to->name),
                        static_cast<double>(dist_sum) / (settings_.bus_velocity * (100.0 / 6.0)) });

                    if (!bus->is_roundtrip) {
                        graph.AddEdge({
                            j - i,
                            bus->name,
                            stops_id_.at(stop_to->name) + 1,
                            stops_id_.at(stop_from->name),
                            static_cast<double>(dist_sum_inverse) / (settings_.bus_velocity * (100.0 / 6.0)) });
                    }
                }
            }
        });
    return graph;
}


const std::optional<graph::Router<double>::RouteInfo> TransportRouter::FindRoute(const std::string_view stop_from, const std::string_view stop_to) const{
    return router_.BuildRoute(stops_id_.at(stop_from.data()), stops_id_.at(stop_to.data()));
}

const graph::Edge<double> TransportRouter::GetEdge(const graph::VertexId& edge_id)const {
    return router_.GetGraph().GetEdge(edge_id);
=======
            });
        ++vertex_id;
    }
    stops_id_ = std::move(stops_id);
    return graph;
}

void TransportRouter::FillBusesGraph(const tc::TransportCatalogue& catalogue, graph::DirectedWeightedGraph<double>& graph){
    const auto& buses = catalogue.GetAllBusses();
    const double sec_mult = 100.0 / 6.0;

    for (const auto& [bus_name, bus_ptr] : buses) {
        const auto& stops = bus_ptr->stops;
        size_t stops_count = stops.size();
        for (size_t i = 0; i < stops_count; ++i) {
            for (size_t j = i + 1; j < stops_count; ++j) {
                const tc::Stop* stop_from = stops[i];
                const tc::Stop* stop_to = stops[j];
                int dist_sum = 0;
                int dist_sum_inverse = 0;
                for (size_t k = i + 1; k <= j; ++k) {
                    dist_sum += catalogue.GetDistanceStop(stops[k - 1], stops[k]);
                    dist_sum_inverse += catalogue.GetDistanceStop(stops[k], stops[k - 1]);
                }
                graph.AddEdge({
                    j - i,
                    bus_name.data(),
                    stops_id_.at(stop_from->name) + 1,
                    stops_id_.at(stop_to->name),
                    static_cast<double>(dist_sum) / (settings_.bus_velocity * sec_mult) });

                if (!bus_ptr->is_roundtrip) {
                    graph.AddEdge({
                        j - i,
                        bus_name.data(),
                        stops_id_.at(stop_to->name) + 1,
                        stops_id_.at(stop_from->name),
                        static_cast<double>(dist_sum_inverse) / (settings_.bus_velocity * sec_mult) });
                }
            }
        }
    }
>>>>>>> d924e7d (2)
}

