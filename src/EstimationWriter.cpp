#include "EstimationWriter.h"

#include <fstream>
#include <iomanip>
#include <stdexcept>

namespace attitude {

void EstimationWriter::write(const std::string& path,
                             const std::vector<EstimationRecord>& rows) const {
    std::ofstream out(path);
    if (!out) {
        throw std::runtime_error("Could not open output file: " + path);
    }

    out << std::fixed << std::setprecision(4);
    for (const auto& r : rows) {
        out << r.timestampMs << "; "
            << r.attitude.pitchDeg << "; "
            << r.attitude.rollDeg << '\n';
    }
}

} // namespace attitude
