#pragma once
#include <string_view>
#include <string>
#include <vector>
#include "type_data.h"

extern const int minHeight;

bool is_number_row(std::string_view str);

std::vector<double> string_to_vector(std::string_view str);

bool check_upper_height(PresTempHum obj);

PresTempHum filter_row(std::vector<double>& row);

std::string::size_type search_for_observation_time(std::string_view str);