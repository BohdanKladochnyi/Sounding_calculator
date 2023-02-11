#pragma once
#include "type_data.h"

#include <vector>

void insert_interpolating(std::vector<PresTempHum>& sounde, double height);

void supplement_sounding(std::vector<PresTempHum>& sounde);

Components calculate_components(std::vector<PresTempHum>& sounde, GNSS_station station);

bool check_upper_height(PresTempHum obj);
