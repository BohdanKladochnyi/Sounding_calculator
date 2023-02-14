#include "user_input.h"

#include <string>
#include <iostream>

#include "type_data.h"

static double numbers_input() {
    double res;

    while (true) {
        std::cin >> res;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(32767, '\n');

            std::cout << "Invaid input. Please, try again: ";
            continue;
        }
        else {
            std::cin.ignore(32767, '\n');

            return res;
        }
    }

}

static std::wstring get_digits(size_t count) {
    std::wstring res;
    while (true) {
        std::wcin >> res;
        std::wcin.ignore(32767, '\n');

        if (res.size() != count) {
            std::cout << "Invaid input. Please, try again: ";
            continue;
        }

        bool err = false;
        for (size_t i = 0; i < count; i++) {
            if (!std::isdigit(res[i])) {
                err = true;
                break;
            }
        }
        if (err) {
            std::cout << "Invaid input. Please, try again: ";
            continue;
        }

        return res;
    }
}

GNSS_station input_station_parameters() {
    GNSS_station station;
    std::wstring hour;

    std::cout << "Enter observation hour (00, 06, 12, 18): ";
    hour = get_digits(2);
    station.hour = std::stoi(hour);


    std::cout << "Enter GNSS station height (meters): ";
    station.height = numbers_input();

    std::cout << "Enter GNSS station lattitude (decimal degrees): ";
    station.phi = numbers_input();

    return station;
}

Input get_input() {
    Input res;
    std::wstring station, year, month, date_from, date_to;

    std::cout << "Enter station number: ";
    station = get_digits(5);

    std::cout << "Enter a year: ";
    year = get_digits(4);

    std::cout << "Enter a month (01, 02 ... 12): ";
    month = get_digits(2);

    std::cout << "Enter start date (01, 02, ... 31): ";
    date_from = get_digits(2);

    std::cout << "Enter final date (01, 02, ... 31: ";
    date_to = get_digits(2);

    res.in_filename = station + L"_" + year + L"_" + month + L"_" + date_from + L"_" + date_to + L"_sounding.txt";
    res.out_filename = station + L"_" + year + L"_" + month + L"_" + date_from + L"_" + date_to + L"_result.txt";

    res.calculation_filename = station + L"_" + year + L"_" + month + L"_" + date_from + L"_" + date_to + L"_calculation.txt";

    res.url = L"https://weather.uwyo.edu/cgi-bin/sounding?region=naconf&TYPE=TEXT%3ALIST&YEAR="
        + year + L"&MONTH=" + month + L"&FROM=" + date_from + L"00&TO=" + date_to + L"18&STNM=" + station;

    return res;
}
