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

#if 1
    // the URL to download from 
    std::wstring url_addr = input.url;

    // the destination file 
    std::wstring destFile = input.in_filename;

    // URLDownloadToFile returns S_OK on success 
    if (S_OK == URLDownloadToFile(NULL, url_addr.c_str(), destFile.c_str(), 0, NULL)) {
        std::wcout << L"--------------------------------------------------------\n";
        std::wcout << L"Sounding data saved to '" << input.in_filename << L"'\n";
    } else {
        std::cerr << "Cannot connect to URL address or invalid date or station number\n";
        return -1;
    }
#endif

#if 1
    std::fstream fin;

    std::vector<Components> calculated_components;
    calculated_components.reserve(31);


    fin.open(input.in_filename, std::ios::in);
    
    if (fin.is_open()) {
        std::string tp;
        
        int current_day = -1;
        std::vector<PresTempHum> sounde;
        sounde.reserve(128);

        std::fstream calc_out;
        calc_out.open(input.calculation_filename, std::ios::out);
        if (!calc_out.is_open()) {
            std::cerr << "Creating file error\n";
            fin.close();
            return -1;
        }

        while (std::getline(fin, tp)) {
            auto pos = tp.find("Observations at ");
            int hour = -1;

            while (hour != station.hour) {
                std::getline(fin, tp);
                if (fin.eof())
                    break;
                pos = tp.find("Observations at ");

                if (pos != std::string::npos) {
                    hour = std::stoi(&tp[pos + 16]); //plus size of 'Observations at '
                    current_day = std::stoi(&tp[pos + 20]);
                }
            }
            
            if (fin.eof())
                break;

            while (!is_number_row(tp))
                std::getline(fin, tp);

            while (is_number_row(tp)) {
                std::vector<double> row = string_to_vector(tp);
                if (row.size() == 11)
                    sounde.push_back(filter_row(row));
                std::getline(fin, tp);
            }

            if (!check_upper_height(sounde.back())) {
                std::wcout << L"Upper sounding height is lower then " << minHeight 
                    << L" at day " << current_day << '\n';
                sounde.clear();
                continue;
            }

            supplement_sounding(sounde);
            insert_interpolating(sounde, station.height);

            calculated_components.push_back(calculate_components(sounde, station));
            calculated_components.back().hour = station.hour;
            calculated_components.back().day = current_day;

            calc_out << "day " << current_day << " hour " << hour << "\n";
            calc_out << "     P          H       T(C)     U       e      T(K)      Nd       Nw       Dd      Dw       Zw\n";
            auto float_comp = [](double a, double b, double epsilon = 0.01) { return std::fabs(a - b) <= epsilon; };
            for (auto& el : sounde) {
                if (el.H > station.height || float_comp(el.H, station.height))
                    el.dump_all(calc_out);
            }
            calc_out << "\n";

            sounde.clear();
        }
        fin.close();
        std::wcout << L"Calculations saved to '" << input.calculation_filename << L"'\n";
        calc_out.close();

    } else {
        std::cerr << "Opening file error\n";
        return -1;
    }
#endif

    std::fstream res_out;
    res_out.open(input.out_filename, std::ios::out);
    if (res_out.is_open()) {
        res_out << "day d_h aer  d_w aer  d_h SA  d_w SA\n";
        for (auto& el : calculated_components) {
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