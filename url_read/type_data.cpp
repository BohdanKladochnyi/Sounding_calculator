#include "type_data.h"

#include <string>
#include <ostream>
#include <iomanip>


void PresTempHum::dump(std::ostream& os) const {
    os << std::setprecision(3) << std::fixed;
    os << P << "    " << H << "    " << T << "    " << U << "    ";
}

void PresTempHum::dump_all(std::ostream& os) const {
    os << std::setprecision(3) << std::fixed;
    os << P << "  " << H << "  " << T << "  " << U << "  "
        << e << "  " << T_k << "  " << N_d << "  " << N_w << "  "
        << D_d << "  " << D_w << "  ";
    os << std::setprecision(6) << std::fixed;
    os << Z_w << "\n";
}

void Components::dump(std::ostream& os) const {
    os << std::setprecision(2) << std::fixed;
    os << day << "    " << hydrostatic << "    " << wet << "    "
        << hydrostatic_SA << "    " << wet_SA << "\n";
}