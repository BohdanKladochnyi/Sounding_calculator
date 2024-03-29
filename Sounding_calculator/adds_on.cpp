#include "adds_on.h"

#include <string_view>
#include <string>
#include <vector>

#include "type_data.h"

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
    row.reserve(5);

    size_t space_counter = 0;
    while (start != str.cend()) {
        while (std::isspace(*start)) {
            ++start;
            ++space_counter;
            if (start == str.cend())
                break;
        }
        if (space_counter > 6) break;
        if (start == str.cend())
            break;

        fin = start;
        while (!std::isspace(*fin)) {
            ++fin;
            if (fin == str.cend())
                break;
        }

        number = std::stof(&(*start));
        row.push_back(number);
        if (row.size() == 5) break;
        
        if (fin == str.cend())
            break;

        start = fin;
        space_counter = 0;
    }
    
    return row;
}

bool check_upper_height(PresTempHum obj) {
    if (obj.H < minHeight)
        return false;
    return true;
    
    //maybe should rewrite as follows with 'file seek'
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
PresTempHum filter_short_row(std::vector<double>& row) {
    PresTempHum obj = { row[0], row[1], row[2], 0.0 };
    return obj;
}