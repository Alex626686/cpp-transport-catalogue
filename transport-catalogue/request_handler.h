#pragma once
#include <optional>
#include <unordered_set>

#include "map_renderer.h"
#include "transport_router.h"

class RequestHandler {
public:
	RequestHandler(const tc::TransportCatalogue& catalogue, render::RenderSettings render_settings, RouterSettings router_settings)
		: catalogue_(catalogue), render_(std::move(render_settings)), router_(router_settings, catalogue) {}

	const tc::TransportCatalogue& GetCatalogue() const;
	const TransportRouter& GetRouter() const;

private:
	const tc::TransportCatalogue& catalogue_;
	render::MapRenderer render_;
	TransportRouter router_;
	
};