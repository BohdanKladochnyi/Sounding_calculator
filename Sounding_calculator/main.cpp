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
    input_init_values();

    while (std::cin) {
        Input input = get_input();
        GNSS_station station = input_station_parameters();

        for (int i = 1; i <= 12; ++i) {
            if (i < 10) {
                input.url[90] = L'0' + i;
            }
            else {
                input.url[89] = L'0' + 1;
                input.url[90] = L'0' + i % 10;
            }

            if (i == 2) {
                input.url[105] = L'0' + 2;
                input.url[106] = L'0' + 8;
            }
            else {
                input.url[105] = L'0' + 3;
                input.url[106] = L'0' + 0;
            }

            std::wstring url_addr = input.url;
            std::wstring destFile = input.in_filename;

            while (!S_OK == URLDownloadToFile(NULL, url_addr.c_str(), destFile.c_str(), 0, NULL))
                std::cerr << "Cannot connect to URL address or invalid date or station number\n";
        
            std::wcout << L"--------------------------------------------------------\n";
            std::wcout << L"Sounding data saved to '" << input.in_filename << L"'\n";
            
            std::vector<Components> tropo_delay;
            tropo_delay.reserve(31);

            std::fstream input_soundings;
            input_soundings.open(input.in_filename, std::ios::in);

        if (input_soundings.is_open()) {
            std::string tp;
            int current_day = -1;

            std::vector<PresTempHum> sounding;
            sounding.reserve(128);

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
                    if (row.size() == 5)
                        sounding.push_back(filter_row(row));
                    if (row.size() == 3 && row[1] > 10000.0)
                        sounding.push_back(filter_short_row(row)); //if humidity is absent
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
                tropo_delay.back().T = sounding[0].T;

                sounding.clear();
            }

            input_soundings.close();

        } else {
            std::cerr << "Opening file error\n";
            return -1;
        }

            //dump in result file
            std::fstream res_out;
            res_out.open(input.out_filename, std::ios::app);
            
            if (res_out.is_open()) {
                if (i == 1) res_out << "day\td_h_aer\td_w_aer\td_h_SA\td_w_SA\tT\n";
                for (auto& component : tropo_delay) {
                    component.dump(res_out);
                }
                res_out << "\n";

                std::wcout << L"Result saved to '" << input.out_filename << L"'\n";
                std::wcout << L"--------------------------------------------------------\n";

                res_out.close();
            }
            else {
                std::cerr << "Creating file error\n";
                return -1;
            }

        }
    }
    //system("pause");
    return 0;
}