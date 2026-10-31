#pragma once

#include "AttitudeEstimator.h"

#include <cstdint>
#include <string>
#include <vector>

namespace attitude {

/** One row of the output log: timestamp + estimated angles. */
struct EstimationRecord {
    std::int64_t timestampMs = 0;
    Attitude attitude;
};

/**
 * Writes "<t>; <pitchDeg>; <rollDeg>" with 4 decimal places,
 * matching the original output_estimations.log format.
 */
class EstimationWriter {
public:
    void write(const std::string& path, const std::vector<EstimationRecord>& rows) const;
};

} // namespace attitude
