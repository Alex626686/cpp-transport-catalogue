#include "map_renderer.h"

namespace render {

inline const double EPSILON = 1e-6;
bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

class SphereProjector {
public:
    // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
        double max_width, double max_height, double padding)
        : padding_(padding) //
    {
        // Если точки поверхности сферы не заданы, вычислять нечего
        if (points_begin == points_end) {
            return;
        }

        // Находим точки с минимальной и максимальной долготой
        const auto [left_it, right_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        // Находим точки с минимальной и максимальной широтой
        const auto [bottom_it, top_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        // Вычисляем коэффициент масштабирования вдоль координаты x
        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        // Вычисляем коэффициент масштабирования вдоль координаты y
        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            // Коэффициенты масштабирования по ширине и высоте ненулевые,
            // берём минимальный из них
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        }
        else if (width_zoom) {
            // Коэффициент масштабирования по ширине ненулевой, используем его
            zoom_coeff_ = *width_zoom;
        }
        else if (height_zoom) {
            // Коэффициент масштабирования по высоте ненулевой, используем его
            zoom_coeff_ = *height_zoom;
        }
    }

    // Проецирует широту и долготу в координаты внутри SVG-изображения
    svg::Point operator()(geo::Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }

private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};

void MapRenderer::GetBusLines(const std::map<std::string_view, const tc::Bus*>& buses, SphereProjector& cp, svg::Document& doc) const{
    using namespace svg;
    int color_counter = 0;
    for (const auto& [name, bus] : buses) {
        if (bus->stops.empty()) continue;
        Polyline line;

        std::vector<tc::Stop*> stops = bus->stops;
        if (!bus->is_roundtrip) {
            stops.reserve(stops.size() * 2);
            stops.insert(stops.end(), std::next(stops.rbegin()), stops.rend());
        }
        
        for (const tc::Stop* stop : stops) {
            line.AddPoint(cp(stop->cords));
        }
        line.SetStrokeColor(renderer_settings_.color_palette[color_counter]);
        line.SetFillColor("none");
        line.SetStrokeLineCap(StrokeLineCap::ROUND);
        line.SetStrokeLineJoin(StrokeLineJoin::ROUND);
        line.SetStrokeWidth(renderer_settings_.line_width);
        
        doc.Add(line);
        if (color_counter < renderer_settings_.color_palette.size() - 1) {
            ++color_counter;
        }
        else {
            color_counter = 0;
        }
    }
}

void MapRenderer::GetBusNames(const std::map<std::string_view, const tc::Bus*>& buses, SphereProjector& sp, svg::Document& doc) const{
    using namespace svg;
    int color_counter = 0;

    for (const auto& [name, bus] : buses) {
        if (bus->stops.empty()) continue;
        Text text;
        text.SetPosition(sp(bus->stops.at(0)->cords));
        text.SetOffset(renderer_settings_.bus_label_offset);
        text.SetFontSize(renderer_settings_.bus_label_font_size);
        text.SetFontFamily("Verdana");
        text.SetFontWeight("bold");
        text.SetData(bus->name);
        text.SetFillColor(renderer_settings_.color_palette[color_counter]);
        if (color_counter < renderer_settings_.color_palette.size() - 1) {
            ++color_counter;
        }
        else {
            color_counter = 0;
        }

        Text underlayer;
        underlayer.SetPosition(sp(bus->stops[0]->cords));
        underlayer.SetOffset(renderer_settings_.bus_label_offset);
        underlayer.SetFontSize(renderer_settings_.bus_label_font_size);
        underlayer.SetFontFamily("Verdana");
        underlayer.SetFontWeight("bold");
        underlayer.SetData(bus->name);
        underlayer.SetFillColor(renderer_settings_.underlayer_color);
        underlayer.SetStrokeColor(renderer_settings_.underlayer_color);
        underlayer.SetStrokeWidth(renderer_settings_.underlayer_width);
        underlayer.SetStrokeLineCap(StrokeLineCap::ROUND);
        underlayer.SetStrokeLineJoin(StrokeLineJoin::ROUND);

        doc.Add(underlayer);
        doc.Add(text);

        if (!bus->is_roundtrip && bus->stops.at(0) != bus->stops.at(bus->stops.size() - 1)) {
            Text text2 = text;
            Text underlayer2 = underlayer;
            text2.SetPosition(sp(bus->stops.at(bus->stops.size() - 1)->cords));
            underlayer2.SetPosition(sp(bus->stops.at(bus->stops.size() - 1)->cords));

            doc.Add(underlayer2);
            doc.Add(text2);
        }
    }
}

void MapRenderer::GetStopsPoints(const std::map<std::string_view, const tc::Stop*>& stops, SphereProjector& sp, svg::Document& doc) const{

    for (const auto& [stop_name, stop] : stops) {
        svg::Circle point;

        point.SetCenter(sp(stop->cords));
        point.SetRadius(renderer_settings_.stop_radius);
        point.SetFillColor("white");

        doc.Add(point);
    }
}

void MapRenderer::GetStopsNames(const std::map<std::string_view, const tc::Stop*>& stops, SphereProjector& sp, svg::Document& doc) const{
    using namespace svg;
    Text text;
    for (const auto& [stop_name, stop] : stops) {
        text.SetPosition(sp(stop->cords));
        text.SetOffset(renderer_settings_.stop_label_offset);
        text.SetFontSize(renderer_settings_.stop_label_font_size);
        text.SetFontFamily("Verdana");
        text.SetData(stop->name);
        text.SetFillColor("black");

        svg::Text underlayer;
        underlayer.SetPosition(sp(stop->cords));
        underlayer.SetOffset(renderer_settings_.stop_label_offset);
        underlayer.SetFontSize(renderer_settings_.stop_label_font_size);
        underlayer.SetFontFamily("Verdana");
        underlayer.SetData(stop->name);
        underlayer.SetFillColor(renderer_settings_.underlayer_color);
        underlayer.SetStrokeColor(renderer_settings_.underlayer_color);
        underlayer.SetStrokeWidth(renderer_settings_.underlayer_width);
        underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        doc.Add(underlayer);
        doc.Add(text);
    }
}


svg::Document MapRenderer::GetSvg(const std::map<std::string_view, const tc::Bus*> buses) const{
    svg::Document res;
    std::vector<geo::Coordinates> stops_cords;
    std::map<std::string_view, const tc::Stop*> stops;

    for (const auto& [name, bus] : buses) {
        if (bus->stops.empty()) continue;
        for (const auto& stop : bus->stops) {
            stops_cords.push_back(stop->cords);
            stops.emplace(stop->name, stop);
        }
    }
    //PointInputIt points_begin, PointInputIt points_end,    double max_width, double max_height, double padding
    SphereProjector sp(stops_cords.begin(), stops_cords.end(), renderer_settings_.width, renderer_settings_.height, renderer_settings_.padding);

    GetBusLines(buses, sp, res);
    GetBusNames(buses, sp, res);
    GetStopsPoints(stops, sp, res);
    GetStopsNames(stops, sp, res);
    return res;
}



}//render