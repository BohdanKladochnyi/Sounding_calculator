#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <cmath>

#include <charconv>
#include <iomanip>

#pragma comment(lib, "Urlmon.lib")

constexpr int minHeight = 28500;
double gnss_station_height;
double gnss_station_phi; //lattitude
int observ_hour;

struct PresTempHum final {
    double P;
    double H;
    double T;
    double U;
    
    double e, T_k, N_d, N_w, D_d, D_w, Z_w;

    void dump(std::ostream& os) const {
        os << P << '\t' << H << '\t' << T << '\t' << U << '\n';
    }

    void dump_all(std::ostream& os) const {
        os << std::setprecision(2) << std::fixed;
        os << P << '\t' << H << '\t' << T << '\t' << U << '\t'
            << e << '\t' << T_k << '\t' << N_d << '\t' << N_w << '\t'
            << D_d << '\t' << D_w << '\t' << Z_w << '\n';
    }
};

struct Components final {
    double hydrostatic;
    double wet;
    double hydrostatic_SA;
    double wet_SA;

    int hour, day;

    void dump(std::ostream& os) const {
        os << hydrostatic << '\t' << wet << '\t' << hydrostatic_SA << '\t' << wet_SA << '\n';
    }
};

struct Input final {
    std::wstring in_filename;
    std::wstring out_filename;
    std::wstring url;
};

bool is_number_row(std::string_view str) {
    if (str.empty()) return 0;
    size_t sign_counter = 0;
    for (auto& c: str) {
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
    PresTempHum obj = { row[0], row[1], row[2], row[4]};
    return obj;
}

std::wstring get_digits(size_t count) {
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

    res.in_filename = station + L"_" + year + month + date_from + L"_" + date_to + L"_sounding.txt";
    res.out_filename = station + L"_" + year + month + date_from + L"_" + date_to + L"_calculation.txt";

    res.url = L"https://weather.uwyo.edu/cgi-bin/sounding?region=naconf&TYPE=TEXT%3ALIST&YEAR="
        + year + L"&MONTH=" + month + L"&FROM=" + date_from + L"00&TO=" + date_to + L"18&STNM=" + station;

    return res;
}

int convert_hours(std::string_view str, std::string::size_type pos) {
    int result = 0;
    std::from_chars(&str[pos], &str[pos + 2], result);
    return result;
}

int hours_input() {
    std::string res;
    int hour;
    while (true) {
        std::cin >> res;
        std::cin.ignore(32767, '\n');

        if (res.size() != 2) {
            std::cout << "Invaid input. Please, try again: ";
            continue;
        }

        bool err = false;
        for (size_t i = 0; i < 2; i++) {
            if (!std::isdigit(res[i])) {
                err = true;
                break;
            }
        }
        if (err) {
            std::cout << "Invaid input. Please, try again: ";
            continue;
        }

        res.append(" "); //dummy fix for next function call
        hour = convert_hours(res, 0);

        if ((hour != 0) && (hour != 6) && (hour != 12) && (hour != 18)) {
            std::cout << "Invaid input. Please, try again: ";
            continue;
        }

        return hour;
    }
}

double numbers_input() {
    double res;

    while (true) {
        std::cin >> res;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(32767, '\n');

            std::cout << "Invaid input. Please, try again: ";
            continue;
        } else {
            std::cin.ignore(32767, '\n');
            
            return res;
        }
    }
    
}

void input_station_parameters() {
    std::cout << "Enter observation hour (00, 06, 12, 18): ";
    observ_hour = hours_input();

    std::cout << "Enter GNSS station height (meters): ";
    gnss_station_height = numbers_input();

    std::cout << "Enter GNSS station lattitude (decimal degrees): ";
    gnss_station_phi = numbers_input();

}

std::string::size_type search_for_observation_time(std::string_view str) {
    return str.find("Observations at ");
}

PresTempHum interpolate(PresTempHum fst, PresTempHum scnd) {
    const double e = 2.718;
    const double g = 9.81;
    const double Rd = 287.0;

    PresTempHum res;
    res.H = gnss_station_height;
    res.T = (res.H - fst.H) * (scnd.T - fst.T) /
        (scnd.H - fst.H) + fst.T;
    res.U = (res.H - fst.H) * (scnd.U - fst.U) /
        (scnd.H - fst.H) + fst.U;
    double Tm = (fst.T + res.T) / 2 + 273.15;
    res.P = fst.P * std::pow(e, -g / (Tm * Rd) * (res.H - fst.H));

    return res;
}

void insert_interpolating(std::vector<PresTempHum>& sounde) {
    size_t i = 0;
    PresTempHum res;

    if (sounde[0].H > gnss_station_height) {
        res = interpolate(sounde[0], sounde[1]);
        sounde.insert(sounde.begin(), res);
        return;
    }

    while (sounde[i].H < gnss_station_height)
        ++i;

    sounde[i - 1] = interpolate(sounde[i - 1], sounde[i]);
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
    while (SMA[i].P > sounde.back().P)
        ++i;
    ++i;
    while (i < 25) {
        sounde.push_back(SMA[i]);
        ++i;
    }

}

Components calculate_components(std::vector<PresTempHum>& sounde) {
    Components res = { 0, 0, 0, 0 };
    size_t i;

    for (i = 0; i != sounde.size(); ++i) {
        sounde[i].e = sounde[i].U / 100.0 * 6.11 * std::pow(10.0,
            ((8.62 * sounde[i].T) / (273.15 + sounde[i].T)));
        
        sounde[i].T_k = sounde[i].T + 273.15;

        sounde[i].N_d = 77.624 * sounde[i].P / sounde[i].T_k *
            (1 - 0.378 * sounde[i].e / sounde[i].P);
        
        sounde[i].Z_w = 1 + 1650.0 * (sounde[i].e / std::pow(sounde[i].T_k, 3)) *
            (1.0 - 0.01317 * sounde[i].T + 1.75 * std::pow(10.0, -4) *
            std::pow(sounde[i].T, 2) + 1.44 * std::pow(10.0, -6) *
            std::pow(sounde[i].T, 3));

        sounde[i].N_w = ((64.7 - 77.624 * 0.622) * sounde[i].e / sounde[i].T_k +
            371900.0 * sounde[i].e / std::pow(sounde[i].T_k, 2)) * sounde[i].Z_w;
    }

    for (i = 0; i != sounde.size() - 1; ++i) {
        sounde[i].D_d = (sounde[i].N_d + sounde[i + 1].N_d) / 2.0 *
            (sounde[i + 1].H - sounde[i].H) / 1000.0;

        sounde[i].D_w = (sounde[i].N_w + sounde[i + 1].N_w) / 2.0 *
            (sounde[i + 1].H - sounde[i].H) / 1000.0;
    }

    auto float_comp = [](double a, double b, double epsilon = 0.01) { return std::fabs(a - b) <= epsilon; };
    i = 0;
    while (!float_comp(sounde[i].H, gnss_station_height))
        ++i;

    double phi = gnss_station_phi * 3.14159 / 180.0;

    res.hydrostatic_SA = 0.002277 * sounde[i].P / (1 - 0.0026 * std::cos(2.0 * phi) -
        28.0 * std::pow(10.0, -8) * gnss_station_height) * 1000.0;

    res.wet_SA = (0.002277 * (1255.0 / sounde[i].T_k + 0.05) * sounde[i].e) * 1000.0;

    for ( ; i != sounde.size(); ++i) {
        res.hydrostatic += sounde[i].D_d;
        res.wet += sounde[i].D_w;
    }

    return res;
}

int main() {
    Input input = get_input();
    input_station_parameters();

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
    calculated_components.reserve(10);


    fin.open(input.in_filename, std::ios::in);
    
    if (fin.is_open()) {
        std::string tp;
        
        int current_day = -1;
        std::vector<PresTempHum> sounde;
        sounde.reserve(128);

        while (std::getline(fin, tp)) {
            auto pos = search_for_observation_time(tp);
            int hour = -1;

            while (hour != observ_hour) {
                std::getline(fin, tp);
                if (fin.eof())
                    break;
                pos = search_for_observation_time(tp);

                if (pos != std::string::npos) {
                    hour = convert_hours(tp, pos + 16); //plus size of 'Observations at '
                    current_day = convert_hours(tp, pos + 20); //yes, works also with days
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
                std::cout << "Upper sounding height is lower then " << minHeight 
                    << " at day " << current_day << '\n';
                continue;
            }

            supplement_sounding(sounde);
            insert_interpolating(sounde);

            calculated_components.push_back(calculate_components(sounde));
            calculated_components.back().hour = observ_hour;
            calculated_components.back().day = current_day;

            sounde.clear();
        }
        fin.close();

    } else {
        std::cerr << "Opening file error\n";
        return -1;
    }
#endif

    std::fstream fout;
    fout.open(input.out_filename, std::ios::out);
    if (fout.is_open()) {
        fout << "d_h aer    d_w aer    d_h SA    d_w SA\n";
        for (auto& el : calculated_components) {
            fout << std::setprecision(2) << std::fixed;
            fout << el.hydrostatic << "    " << el.hydrostatic_SA << "    "
                << el.wet << "    " << el.wet_SA << "\n";
        }
        std::wcout << L"Calculation saved to '" << input.out_filename << L"'\n";
        std::wcout << L"--------------------------------------------------------\n";
        fout.close();
    } else {
        std::cerr << "Creating file error\n";
        return -1;
    }

    system("pause");
    return 0;
}