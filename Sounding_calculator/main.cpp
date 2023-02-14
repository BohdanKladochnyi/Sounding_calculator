#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "user_input.h"
#include "calculation.h"
#include "type_data.h"
#include "adds_on.h"

#pragma comment(lib, "Urlmon.lib")


int main() {
    Input input = get_input();
    GNSS_station station = input_station_parameters();

    std::wstring url_addr = input.url;
    std::wstring destFile = input.in_filename;

    if (S_OK == URLDownloadToFile(NULL, url_addr.c_str(), destFile.c_str(), 0, NULL)) {
        std::wcout << L"--------------------------------------------------------\n";
        std::wcout << L"Sounding data saved to '" << input.in_filename << L"'\n";
    } else {
        std::cerr << "Cannot connect to URL address or invalid date or station number\n";
        return -1;
    }

    std::vector<Components> tropo_delay;
    tropo_delay.reserve(31);

    std::fstream input_soundings;
    input_soundings.open(input.in_filename, std::ios::in);
    
    if (input_soundings.is_open()) {
        std::string tp;
        int current_day = -1;
        
        std::vector<PresTempHum> sounding;
        sounding.reserve(128);

        std::fstream calc_out;
        calc_out.open(input.calculation_filename, std::ios::out);
        if (!calc_out.is_open()) {
            std::cerr << "Creating file error\n";
            input_soundings.close();
            return -1;
        }

        while (std::getline(input_soundings, tp)) {
            auto pos = tp.find("Observations at ");
            int hour = -1;

            //searching for needed observation hour
            while (hour != station.hour) {
                pos = tp.find("Observations at ");

                if (pos != std::string::npos) {
                    hour = std::stoi(&tp[pos + 16]); //plus size of 'Observations at '
                    current_day = std::stoi(&tp[pos + 20]);
                }

                std::getline(input_soundings, tp);
                if (input_soundings.eof()) break;
            }
            if (input_soundings.eof()) break;

            //searching where sounding data begins
            while (!is_number_row(tp)) {
                std::getline(input_soundings, tp);
                if (input_soundings.eof()) break;
            }
            if (input_soundings.eof()) break;

            //reads all data from certain sounding
            while (is_number_row(tp)) {
                std::vector<double> row = string_to_vector(tp);
                if (row.size() == 11)
                    sounding.push_back(filter_row(row));
                std::getline(input_soundings, tp);
                if (input_soundings.eof()) break;
            }
            if (input_soundings.eof()) break;

            //check if the upper sounding height is suitable
            if (!check_upper_height(sounding.back())) {
                std::wcout << L"Upper sounding height is lower then " << minHeight 
                    << L" at day " << current_day << '\n';
                sounding.clear();
                continue;
            }

            //supplement sounding with SMA data
            supplement_sounding(sounding);
            //interpolate starting point of sounding to GNSS station height
            insert_interpolating(sounding, station.height);

            //calculating tropospheric delay components
            tropo_delay.push_back(calculate_components(sounding, station));
            tropo_delay.back().day = current_day;

            calc_out << "day " << current_day << " hour " << hour << "\n";
            calc_out << "     P          H       T(C)     U       e      T(K)      Nd       Nw       Dd      Dw       Zw\n";
            auto float_comp = [](double a, double b, double epsilon = 0.01) { return std::fabs(a - b) <= epsilon; };
            for (auto& el : sounding) {
                if (el.H > station.height || float_comp(el.H, station.height))
                    el.dump_all(calc_out);
            }
            calc_out << "\n";

            sounding.clear();
        }

        input_soundings.close();
        calc_out.close();
        std::wcout << L"Calculations saved to '" << input.calculation_filename << L"'\n";

    } else {
        std::cerr << "Opening file error\n";
        return -1;
    }

    std::fstream res_out;
    res_out.open(input.out_filename, std::ios::out);
    if (res_out.is_open()) {
        res_out << "day d_h aer  d_w aer  d_h SA  d_w SA\n";
        for (auto& el : tropo_delay) {
            el.dump(res_out);
        }

        std::wcout << L"Result saved to '" << input.out_filename << L"'\n";
        std::wcout << L"--------------------------------------------------------\n";
        
        res_out.close();
    } else {
        std::cerr << "Creating file error\n";
        return -1;
    }

    system("pause");
    return 0;
}