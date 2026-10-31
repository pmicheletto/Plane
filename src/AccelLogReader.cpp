#include "AccelLogReader.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace attitude {

std::vector<AccelRecord> AccelLogReader::read(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("Could not open input file: " + path);
    }

    std::vector<AccelRecord> records;
    skipped_ = 0;

    std::string line;
    while (std::getline(in, line)) {
        // Normalise separators/line endings so a plain stream extraction works.
        std::replace(line.begin(), line.end(), ';', ' ');
        std::replace(line.begin(), line.end(), '\r', ' ');

        if (line.find_first_not_of(' ') == std::string::npos) {
            continue; // blank line
        }

        std::istringstream fields(line);
        AccelRecord rec;
        if (fields >> rec.timestampMs >> rec.accel.x >> rec.accel.y >> rec.accel.z) {
            records.push_back(rec);
        } else {
            ++skipped_;
        }
    }

    return records;
}

} // namespace attitude
