#include "adds_on.h"

#include <string_view>
#include <charconv>
#include <string>
#include <sstream>
#include <vector>

#include "type_data.h"

int convert_hours(std::string_view str, std::string::size_type pos) {
    int result = 0;
    std::from_chars(&str[pos], &str[pos + 2], result);
    return result;
}

bool is_number_row(std::string_view str) {
    if (str.empty()) return 0;
    size_t sign_counter = 0;
    for (auto& c : str) {
        if (!std::isdigit(c) && !std::isspace(c) && (c != '.') && (c != '-'))
            return false;
        if (c == '-')
            ++sign_counter;
        else
            sign_counter = 0;
        if (sign_counter == 2)
            return false;
    }
    return true;
}

std::vector<double> string_to_vector(const std::string& str) {
    double number;
    std::vector<double> row;
    row.reserve(11);
    std::stringstream ss(str);
    while (ss >> std::skipws >> number)
        row.push_back(number);
    return row;
}

bool check_upper_height(PresTempHum obj) {
    if (obj.H < minHeight)
        return false;
    return true;
    //remember position
    //read to last row
    //read row to vector
    //check height
    //put back old position
}

PresTempHum filter_row(std::vector<double>& row) {
    PresTempHum obj = { row[0], row[1], row[2], row[4] };
    return obj;
}

std::string::size_type search_for_observation_time(std::string_view str) {
    return str.find("Observations at ");
}
