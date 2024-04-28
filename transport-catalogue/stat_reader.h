#pragma once

#include <iosfwd>
#include <string_view>
#include <iostream>
#include <algorithm>
#include <set>

#include "transport_catalogue.h"

void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request,
                       std::ostream& output);

int CountUniqueStops(const Bus& bus);
double ComputeRoute(const Bus& bus);