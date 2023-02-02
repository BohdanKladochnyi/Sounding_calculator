#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <charconv>

#pragma comment(lib, "Urlmon.lib")

constexpr int minHeight = 28500;

struct PresTempHum {
    double pressure;
    double heigth;
    double temperature;
    double hummidity;
    
    void dump() {
        std::cout << pressure << '\t' << heigth << '\t' << temperature << '\t' << hummidity << '\n';
    }
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
}

PresTempHum filter_row(std::vector<double>& row) {
    PresTempHum obj = { row[1], row[2], row[3], row[5]};
    return obj;
}

std::wstring url_input() {
    std::wstring station, year, month, date_from, date_to;
    std::wstring url_addr;

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

    url_addr = L"https://weather.uwyo.edu/cgi-bin/sounding?region=naconf&TYPE=TEXT%3ALIST&YEAR="
        + year + L"&MONTH=" + month + L"&FROM=" + date_from + L"00&TO=" + date_to + L"18&STNM=" + station;

    return url_addr;
}

std::string::size_type search_for_observation_time(std::string_view str) {
    return str.find("Observations at ");
}

int convert_hours(std::string_view str, std::string::size_type pos) {
    int result = 0;
    std::from_chars(&str[pos], &str[pos + 2], result);
    return result;
}

int main() {
#if 0
    // the URL to download from 
    std::wstring url_addr = url_input();

    // the destination file 
    std::wstring destFile = L"myfile.txt";

    // URLDownloadToFile returns S_OK on success 
    if (S_OK == URLDownloadToFile(NULL, url_addr.c_str(), destFile.c_str(), 0, NULL)) {
        std::cout << "Saved to 'myfile.txt'\n";
    } else {
        std::cout << "Failed\n";
        return -1;
    }
#endif

#if 1
    std::fstream newfile;
    /*newfile.open("myfile.txt", std::ios::out);
    if (newfile.is_open()) {
        newfile << "Tutorials point \n";
        newfile.close();
    }*/

    std::vector<PresTempHum> sound;
    sound.reserve(24);
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
                    sound.push_back(filter_row(row));
            }
        }
        newfile.close();
    }
    /*for (auto& el : sound)
        el.dump();*/
#endif
}