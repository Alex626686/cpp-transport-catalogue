#pragma once
#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

class JsonReader {
public:
    JsonReader(std::istream& input)
        : input_(json::Load(input))
    {}

    const json::Node& GetBaseRequests() const;

    const json::Node& GetStatRequests() const;

    const json::Node GetRenderSettings() const;

    void FillCatalogue(tc::TransportCatalogue&) const;

    render::RendererSettings FillRenderSettings() const;

    std::tuple<std::string_view, geo::Coordinates, std::map<std::string_view, int>> GetStopVars(const json::Dict& request_map) const;

    std::tuple<std::string_view, std::vector<std::string_view>, bool> GetBusVars(const json::Dict& request_map) const;

    void Print(const tc::TransportCatalogue& catalogue) const;

    json::Node PrintStop(const json::Dict&, const tc::TransportCatalogue&) const;

    json::Node PrintBus(const json::Dict&, const tc::TransportCatalogue&) const;

    json::Node PrintMap(const json::Dict&, const tc::TransportCatalogue&) const;

    

private:
    json::Document input_;
    json::Node node_null_ = nullptr;

};