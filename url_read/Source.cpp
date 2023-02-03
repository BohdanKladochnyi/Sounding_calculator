#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <charconv>
#include <cmath>

#pragma comment(lib, "Urlmon.lib")

constexpr int minHeight = 28500;
int gnss_station_height;
int observ_hour;

struct PresTempHum final {
    double pressure;
    double heigth;
    double temperature;
    double hummidity;
    
    void dump() const {
        std::cout << pressure << '\t' << heigth << '\t' << temperature << '\t' << hummidity << '\n';
    }
};

struct Input final {
    std::wstring filename;
    std::wstring url;
};

bool is_number_row(std::string_view str) {
    if (str.empty()) return 0;
    size_t i = 0;
    for (auto& c: str) {
        if (!std::isdigit(c) && !std::isspace(c) && (c != '.') && (c != '-'))
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
    if (obj.heigth < minHeight)
        return false;
    return true;
    //remember position
    //read to last row
    //read row to vector
    //check height
    //put back old position
}

PresTempHum filter_row(std::vector<double>& row) {
    PresTempHum obj = { row[1], row[2], row[3], row[5]};
    return obj;
}

Input get_input() {
    Input res;
    std::wstring station, year, month, date_from, date_to;

    std::cout << "Enter station number: ";
    std::wcin >> station;

    std::cout << "Enter a year: ";
    std::wcin >> year;

    std::cout << "Enter a month: ";
    std::wcin >> month;

    std::cout << "Enter date from: ";
    std::wcin >> date_from;

    std::cout << "Enter date to: ";
    std::wcin >> date_to;


    res.filename = station + L"_" + year + month + date_from + L".txt";
    res.url = L"https://weather.uwyo.edu/cgi-bin/sounding?region=naconf&TYPE=TEXT%3ALIST&YEAR="
        + year + L"&MONTH=" + month + L"&FROM=" + date_from + L"00&TO=" + date_to + L"18&STNM=" + station;

    return res;
}

std::string::size_type search_for_observation_time(std::string_view str) {
    return str.find("Observations at ");
}

int convert_hours(std::string_view str, std::string::size_type pos) {
    int result = 0;
    std::from_chars(&str[pos], &str[pos + 2], result);
    return result;
}

PresTempHum interpolate(PresTempHum fst, PresTempHum scnd) {
    const double e = 2.718;
    const double g = 9.81;
    const double Rd = 287.0;
    PresTempHum res;
    res.heigth = gnss_station_height;
    res.temperature = (res.heigth - fst.heigth) * (scnd.temperature - fst.temperature) /
        (scnd.heigth - fst.heigth) + fst.temperature;
    res.hummidity = (res.heigth - fst.heigth) * (scnd.hummidity - fst.hummidity) /
        (scnd.heigth - fst.heigth) + fst.hummidity;
    double Tm = (fst.temperature + res.temperature) / 2 + 273.15;
    res.pressure = fst.pressure * std::pow(e, -g / (((fst.temperature + res.temperature) /
        2 + 273.15) * Rd) * (res.heigth - fst.heigth));
    return res;
}

double calculate_hydrostatic(const std::vector<PresTempHum>& sounde) {
    double res = 0.0;

    return res;
}

double calculate_wet(const std::vector<PresTempHum>& sounde) {
    double res = 0.0;

    return res;
}

double calculate_hydrostatic_SA(const std::vector<PresTempHum>& sounde) {
    double res = 0.0;

    return res;
}

double calculate_wet_SA(const std::vector<PresTempHum>& sounde) {
    double res = 0.0;

    return res;
}

int main() {
#if 1
    Input input = get_input();
    // the URL to download from 
    std::wstring url_addr = input.url;

    // the destination file 
    std::wstring destFile = input.filename;

    // URLDownloadToFile returns S_OK on success 
    if (S_OK == URLDownloadToFile(NULL, url_addr.c_str(), destFile.c_str(), 0, NULL)) {
        std::wcout << L"Saved to '" << input.filename << L"'\n";
    } else {
        std::cerr << "Cannot connect to URL address\n";
        return -1;
    }
#endif

#if 1
    std::fstream newfile;
    std::vector<PresTempHum> sounde;
    sounde.reserve(24);

    std::cout << "Enter GNSS station height: ";
    std::cin >> gnss_station_height;
    
    newfile.open("myfile.txt", std::ios::in);
    if (newfile.is_open()) {
        std::string tp;
        std::vector<double> row;
        while (std::getline(newfile, tp)) {
            auto pos = search_for_observation_time(tp);
            int hour;
            if (pos != std::string::npos)
                hour = convert_hours(tp, pos + 16); //plus size of 'Observations at '
            if (is_number_row(tp)) {
                row = string_to_vector(tp);
                if (row.size() == 11)
                    sounde.push_back(filter_row(row));
            }
        }
        newfile.close();
    } else {
        std::cerr << "Opening file error\n";
        return -1;
    }
    /*for (auto& el : sounde)
        el.dump();*/
#endif
    /*newfile.open("myfile.txt", std::ios::out);
    if (newfile.is_open()) {
        newfile << "Tutorials point \n";
        newfile.close();
    }*/
}