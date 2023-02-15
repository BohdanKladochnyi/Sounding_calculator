#include "calculation.h"

#include <cmath>
#include <vector>
#include "type_data.h"


static PresTempHum interpolate(PresTempHum fst, PresTempHum scnd, double height) {
    const double e = 2.718;
    const double g = 9.81;
    const double Rd = 287.0;

    PresTempHum res;
    res.H = height;
    res.T = (res.H - fst.H) * (scnd.T - fst.T) /
        (scnd.H - fst.H) + fst.T;
    res.U = (res.H - fst.H) * (scnd.U - fst.U) /
        (scnd.H - fst.H) + fst.U;
    double Tm = (fst.T + res.T) / 2 + 273.15;
    res.P = fst.P * std::pow(e, -g / (Tm * Rd) * (res.H - fst.H));

    return res;
}

void insert_interpolating(std::vector<PresTempHum>& sounding, double height) {
    size_t i = 0;
    PresTempHum res;

    if (sounding[0].H > height) {
        res = interpolate(sounding[0], sounding[1], height);
        sounding.insert(sounding.begin(), res);
        return;
    }

    while (sounding[i].H < height)
        ++i;

    sounding[i - 1] = interpolate(sounding[i - 1], sounding[i], height);
}

void supplement_sounding(std::vector<PresTempHum>& sounding) {
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
    auto float_comp = [](double a, double b, double epsilon = 0.01) { return std::fabs(a - b) <= epsilon; };

    while (SMA[i].P > sounding.back().P || SMA[i].H < sounding.back().H)
        ++i;

    while (i < 25) {
        sounding.push_back(SMA[i]);
        ++i;
    }

}

Components calculate_components(std::vector<PresTempHum>& sounding, GNSS_station station) {
    Components res = { 0, 0, 0, 0 };
    size_t i;

    for (i = 0; i != sounding.size(); ++i) {
        sounding[i].e = sounding[i].U / 100.0 * 6.11 * std::pow(10.0,
            ((8.62 * sounding[i].T) / (273.15 + sounding[i].T)));

        sounding[i].T_k = sounding[i].T + 273.15;

        sounding[i].N_d = 77.624 * sounding[i].P / sounding[i].T_k *
            (1 - 0.378 * sounding[i].e / sounding[i].P);

        sounding[i].Z_w = 1 + 1650.0 * (sounding[i].e / std::pow(sounding[i].T_k, 3)) *
            (1.0 - 0.01317 * sounding[i].T + 1.75 * std::pow(10.0, -4) *
                std::pow(sounding[i].T, 2) + 1.44 * std::pow(10.0, -6) *
                std::pow(sounding[i].T, 3));

        sounding[i].N_w = ((64.7 - 77.624 * 0.622) * sounding[i].e / sounding[i].T_k +
            371900.0 * sounding[i].e / std::pow(sounding[i].T_k, 2)) * sounding[i].Z_w;
    }

    for (i = 0; i != sounding.size() - 1; ++i) {
        sounding[i].D_d = (sounding[i].N_d + sounding[i + 1].N_d) / 2.0 *
            (sounding[i + 1].H - sounding[i].H) / 1000.0;

        sounding[i].D_w = (sounding[i].N_w + sounding[i + 1].N_w) / 2.0 *
            (sounding[i + 1].H - sounding[i].H) / 1000.0;
    }

    auto float_comp = [](double a, double b, double epsilon = 0.01) { return std::fabs(a - b) <= epsilon; };
    i = 0;
    while (!float_comp(sounding[i].H, station.height))
        ++i;

    double rad_phi = station.phi * 3.14159 / 180.0;

    res.hydrostatic_SA = 0.002277 * sounding[i].P / (1 - 0.0026 * std::cos(2.0 * rad_phi) -
        28.0 * std::pow(10.0, -8) * station.height) * 1000.0;

    res.wet_SA = (0.002277 * (1255.0 / sounding[i].T_k + 0.05) * sounding[i].e) * 1000.0;

    for (; i != sounding.size(); ++i) {
        res.hydrostatic += sounding[i].D_d;
        res.wet += sounding[i].D_w;
    }

    return res;
}
