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
    PresTempHum obj = { row[0], row[1], row[2], row[4]};
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

void supplement_sounding(std::vector<PresTempHum>& sounde) {
    static PresTempHum SMA[25] = {
        { 11.970, 30000.0, -46.641, 0.0 },
        { 10.313, 31000.0, -45.650, 0.0 },
        { 8.891, 32000.0, -44.660, 0.0 },
        { 7.673, 33000.0, -42.177, 0.0 },
        { 6.634, 34000.0, -39.406, 0.0 },
        { 5.746, 35000.0, -36.637, 0.0 },
        { 4.985, 36000.0, -33.868, 0.0 },
        { 4.332, 37000.0, -31.100, 0.0 },
        { 3.771, 38000.0, -28.332, 0.0 },
        { 3.288, 39000.0, -25.566, 0.0 },
        { 2.871, 40000.0, -22.800, 0.0 },
        { 2.511, 41000.0, -20.036, 0.0 },
        { 2.200, 42000.0, -17.272, 0.0 },
        { 1.929, 43000.0, -14.509, 0.0 },
        { 1.695, 44000.0, -11.747, 0.0 },
        { 1.491, 45000.0, -8.986, 0.0 },
        { 1.313, 46000.0, -6.225, 0.0 },
        { 1.158, 47000.0, -3.466, 0.0 },
        { 1.023, 48000.0, -2.500, 0.0 },
        { 0.903, 49000.0, -2.500, 0.0 },
        { 0.798, 50000.0, -2.500, 0.0 },
        { 0.425, 55000.0, -12.379, 0.0 },
        { 0.220, 60000.0, -26.129, 0.0 },
        { 0.109, 65000.0, -39.858, 0.0 },
        { 0.052, 70000.0, -53.565, 0.0 }
    };

    size_t i = 0;
    while (SMA[i].pressure > sounde.back().pressure)
        ++i;
    while (i < 24) {
        sounde.push_back(SMA[i]);
        ++i;
    }

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
#if 0
    Input input = get_input();
    // the URL to download from 
    std::wstring url_addr = input.url;

    // the destination file 
    std::wstring destFile = input.filename;

    // URLDownloadToFile returns S_OK on success 
    if (S_OK == URLDownloadToFile(NULL, url_addr.c_str(), destFile.c_str(), 0, NULL)) {
        std::wcout << L"Sounding data saved to '" << input.filename << L"'\n";
    } else {
        std::cerr << "Cannot connect to URL address\n";
        return -1;
    }
#endif

#if 1
    std::fstream newfile;

    std::cout << "Enter GNSS station height: ";
    std::cin >> gnss_station_height;
    
    std::vector<PresTempHum> sounde;
    sounde.reserve(24);

    newfile.open("myfile.txt", std::ios::in);
    if (newfile.is_open()) {
        std::string tp;
        std::vector<double> row;
        row.reserve(11);

        

        while (std::getline(newfile, tp)) {
            auto pos = search_for_observation_time(tp);
            int hour = -1;

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
    supplement_sounding(sounde);
    for (auto& el : sounde)
        el.dump();
#endif
    /*newfile.open("myfile.txt", std::ios::out);
    if (newfile.is_open()) {
        newfile << "Tutorials point \n";
        newfile.close();
    }*/
}