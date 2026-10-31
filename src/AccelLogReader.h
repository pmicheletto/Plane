#pragma once

#include "AttitudeEstimator.h"

#include <cstdint>
#include <string>
#include <vector>

namespace attitude {

/** One row of the input log: timestamp in ms + raw accelerometer reading. */
struct AccelRecord {
    std::int64_t timestampMs = 0;
    AccelSample accel;
};

/**
 * Reads the "<t>; <ax>; <ay>; <az>" log format.
 *
 * - Tolerates CRLF line endings and a trailing ';'.
 * - Skips blank lines.
 * - Throws std::runtime_error if the file cannot be opened.
 * - Malformed lines are skipped and counted in `skippedLines`.
 */
class AccelLogReader {
public:
    std::vector<AccelRecord> read(const std::string& path);

    std::size_t skippedLines() const { return skipped_; }

private:
    std::size_t skipped_ = 0;
};

} // namespace attitude
