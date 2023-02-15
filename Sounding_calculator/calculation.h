#pragma once
#include "type_data.h"

#include <vector>

void insert_interpolating(std::vector<PresTempHum>& sounding, double height);

void supplement_sounding(std::vector<PresTempHum>& sounding);

Components calculate_components(std::vector<PresTempHum>& sounding, GNSS_station station);

bool check_upper_height(PresTempHum obj);
