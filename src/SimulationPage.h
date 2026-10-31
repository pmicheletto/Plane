#pragma once

#include "AccelLogReader.h"
#include "EstimationWriter.h"

#include <string>
#include <vector>

namespace attitude {

/**
 * Builds the self-contained simulation page: the HTML template embedded in
 * the binary (web/simulation.html) with the computed samples inlined as JSON.
 * The result opens directly from disk — no server, no network.
 */
class SimulationPage {
public:
    /** Returns the full HTML document. */
    static std::string render(const std::vector<AccelRecord>& samples,
                              const std::vector<EstimationRecord>& estimates,
                              const std::string& equationsName,
                              const std::string& sourceName);

    /** Renders and writes to `path`. Throws std::runtime_error on failure. */
    static void write(const std::string& path,
                      const std::vector<AccelRecord>& samples,
                      const std::vector<EstimationRecord>& estimates,
                      const std::string& equationsName,
                      const std::string& sourceName);
};

} // namespace attitude
