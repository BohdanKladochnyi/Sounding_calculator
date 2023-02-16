#include "type_data.h"

#include <ostream>
#include <iomanip>


void PresTempHum::dump(std::ostream& os) const {
    os << std::setprecision(3) << std::fixed;
    os << std::setw(9) << P << std::setw(11) << H
        << std::setw(9) << T << std::setw(8) << U << '\n';
}

void PresTempHum::dump_all(std::ostream& os) const {
    os << std::setprecision(3) << std::fixed;
    os << std::setw(9) << P << std::setw(11) << H << std::setw(9) << T
        << std::setw(8) << U << std::setw(8) << e << std::setw(9) << T_k
        << std::setw(9) << N_d << std::setw(8) << N_w << std::setw(9)
        << D_d << std::setw(8) << D_w;

    os << std::setprecision(6) << std::fixed;
    os << std::setw(10) << Z_w << "\n";
}

void Components::dump(std::ostream& os) const {
    os << std::setprecision(2) << std::fixed;
    os << std::setw(2) << day << std::setw(9) << hydrostatic << std::setw(8) << wet << std::setw(9)
        << hydrostatic_SA << std::setw(8) << wet_SA << "\n";
}

void Components::dump_calc(std::ostream& os) const {
    os << std::setprecision(2) << std::fixed;
    os << "                                                              Aerology:"
        << std::setw(9) << hydrostatic << std::setw(8) << wet << "\n";
    os << "                                                          Saastamoinen:"
        << std::setw(9) << hydrostatic_SA << std::setw(8) << wet_SA << "\n";
}