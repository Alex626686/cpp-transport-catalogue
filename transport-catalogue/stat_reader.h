#pragma once

#include <iosfwd>
#include <string_view>
#include <iostream>
#include <algorithm>
#include <set>

#include "transport_catalogue.h"

void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request,
                       std::ostream& output);

void PrintBus(const TransportCatalogue& transport_catalogue, const Bus& bus, std::ostream& output);
void PrintStop(const std::vector<std::string_view>& buses, std::ostream& output);