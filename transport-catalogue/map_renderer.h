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

struct RenderSettings {
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
	MapRenderer() = default;

	MapRenderer(RenderSettings renderer_settings)
		: renderer_settings_(renderer_settings) {}

	void GetBusLines(const std::map<std::string_view, const tc::Bus*>&, SphereProjector&, svg::Document&) const;

	void GetBusNames(const std::map<std::string_view, const tc::Bus*>&, SphereProjector&, svg::Document&) const;

	void GetStopsPoints(const std::map<std::string_view, const tc::Stop*>&, SphereProjector&, svg::Document&) const;

	void GetStopsNames(const std::map<std::string_view, const tc::Stop*>&, SphereProjector&, svg::Document&) const;

	svg::Document GetSvg(const std::map<std::string_view, const tc::Bus*>) const;

private:
	RenderSettings renderer_settings_;
};

} //render
