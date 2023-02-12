#include "adds_on.h"

#include <string_view>
#include <charconv>
#include <string>
#include <vector>

#include "type_data.h"

const int minHeight = 30000;

int convert_hours(std::string_view str, std::string::size_type pos) {
    int result = 0;
    std::from_chars(&str[pos], &str[pos + 2], result);
    return result;
}

bool is_number_row(std::string_view str) {
    if (str.empty()) return false;
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

std::vector<double> string_to_vector(std::string_view str) {
    double number;
    auto start = str.cbegin();
    auto fin = str.cend();

    std::vector<double> row;
    row.reserve(11);

    /*std::stringstream ss(str);
    while (ss >> std::skipws >> number)
        row.push_back(number);*/

    while (start != str.cend()) {
        while (std::isspace(*start)) {
            ++start;
            if (start == str.cend())
                break;
        }

        if (start == str.cend())
            break;

        fin = start;
        while (!std::isspace(*fin)) {
            ++fin;
            if (fin == str.cend())
                break;
        }

        std::string tp(start, fin);

        number = std::stof(tp);
        row.push_back(number);
        
        if (fin == str.cend())
            break;

        start = fin;
    }
    
    return row;
}

bool check_upper_height(PresTempHum obj) {
    if (obj.H < minHeight)
        return false;
    return true;
    
    //maybe should rewrite as follows with seek
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
