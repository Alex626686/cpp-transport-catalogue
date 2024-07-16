#pragma once
#include "geo.h"
#include "svg.h"
#include "domain.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>
#include <set>
#include <map>

namespace render{

class SphereProjector;

struct RendererSettings {
	double width;
	double height;
	double padding;
	double stop_radius;
	double line_width;
	int bus_label_font_size;
	svg::Point bus_label_offset;
	int stop_label_font_size;
	svg::Point stop_label_offset;
	svg::Color underlayer_color;
	double underlayer_width;
	std::vector<svg::Color> color_palette;
};

class MapRenderer {
public:
	MapRenderer(RendererSettings renderer_settings)
		: renderer_settings_(renderer_settings) {}

	std::vector<svg::Polyline> GetBusLines(const std::map<std::string_view, const tc::Bus*>&, SphereProjector&) const;

	std::vector<svg::Text> GetBusNames(const std::map<std::string_view, const tc::Bus*>&, SphereProjector&) const;

	std::vector<svg::Circle> GetStopsPoints(const std::map<std::string_view, const tc::Stop*>&, SphereProjector&) const;

	std::vector<svg::Text> GetStopsNames(const std::map<std::string_view, const tc::Stop*>&, SphereProjector&) const;

	svg::Document GetSvg(const std::map<std::string_view, const tc::Bus*>) const;

private:
	RendererSettings renderer_settings_;
};

} //render
