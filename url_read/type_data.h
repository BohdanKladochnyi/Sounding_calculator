#pragma once
#include <ostream>

struct PresTempHum final {
    double P;
    double H;
    double T;
    double U;

    double e, T_k, N_d, N_w, D_d, D_w, Z_w;

    void dump(std::ostream& os) const;

    void dump_all(std::ostream& os) const;
};

struct Components final {
    double hydrostatic;
    double wet;
    double hydrostatic_SA;
    double wet_SA;

    int hour, day;

    void dump(std::ostream& os) const;
};

struct Input final {
    std::wstring in_filename;
    std::wstring calculation_filename;
    std::wstring out_filename;
    std::wstring url;
};

struct GNSS_station final {
    double height;
    double phi;
    int hour;
};