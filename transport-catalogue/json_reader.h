#pragma once
#include "json_builder.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"
#include "request_handler.h"



class JsonReader {
public:
    JsonReader(std::istream& input)
        : input_(json::Load(input))
    {}

    const json::Node& GetBaseRequests() const;
    const json::Node& GetStatRequests() const;
    const json::Node& GetRenderSettings() const;
    const json::Node& GetRouterSettings() const;

    void FillCatalogue(tc::TransportCatalogue&) const;
    render::RenderSettings FillRenderSettings() const;
    RouterSettings FillRouterSettings() const;

    std::tuple<std::string_view, geo::Coordinates, std::map<std::string_view, int>> GetStopVars(const json::Dict& request_map) const;
    std::tuple<std::string_view, std::vector<std::string_view>, bool> GetBusVars(const json::Dict& request_map) const;

    void Print(const RequestHandler& handler) const;
    json::Node PrintStop(const json::Dict&, const RequestHandler&) const;
    json::Node PrintBus(const json::Dict&, const RequestHandler&) const;
    json::Node PrintMap(const json::Dict&, const RequestHandler&) const;
    json::Node PrintRoute(const json::Dict&, const RequestHandler&) const;
    

private:
    json::Document input_;
    json::Node node_null_ = nullptr;

};