/**
 * Pitch & Roll — read an accelerometer log, estimate the angles, save them as
 * CSV and open an interactive simulation in the default web browser.
 *
 * Usage:
 *   pitch_roll [input.log] [--an3461] [--no-browser]
 *
 * Outputs (in the current directory):
 *   output_estimations.log   <timestampMs>; <pitchDeg>; <rollDeg>
 *   output_simulation.html   self-contained page, opens from disk
 */
#include "AccelLogReader.h"
#include "AttitudeEstimator.h"
#include "Browser.h"
#include "EstimationWriter.h"
#include "SimulationPage.h"

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

using namespace attitude;

int main(int argc, char** argv) {
    std::string inputPath = "attitude_exam.log";
    const std::string csvPath = "output_estimations.log";
    const std::string htmlPath = "output_simulation.html";
    auto convention = AttitudeEstimator::Convention::Legacy;
    bool openBrowser = true;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--an3461") == 0)         convention = AttitudeEstimator::Convention::AN3461_XYZ;
        else if (std::strcmp(argv[i], "--no-browser") == 0) openBrowser = false;
        else                                                inputPath = argv[i];
    }
    const std::string equationsName =
        convention == AttitudeEstimator::Convention::Legacy ? "Legacy (original program)" : "AN3461 eq. 25/26";

    try {
        // 1. Read
        AccelLogReader reader;
        const std::vector<AccelRecord> samples = reader.read(inputPath);
        std::cout << "Read " << samples.size() << " samples from " << inputPath;
        if (reader.skippedLines()) std::cout << " (" << reader.skippedLines() << " malformed lines skipped)";
        std::cout << "\n";

        // 2. Estimate
        const AttitudeEstimator estimator(convention);
        std::vector<EstimationRecord> estimates;
        estimates.reserve(samples.size());
        for (const auto& s : samples) estimates.push_back({s.timestampMs, estimator.estimate(s.accel)});
        std::cout << "Estimated pitch/roll using " << equationsName << "\n";

        // 3. Save CSV
        EstimationWriter().write(csvPath, estimates);
        std::cout << "Saved " << csvPath << "\n";

        // 4. Build the simulation page and open it
        SimulationPage::write(htmlPath, samples, estimates, equationsName, inputPath);
        std::cout << "Saved " << htmlPath << "\n";

        if (openBrowser) {
            if (openInBrowser(htmlPath)) std::cout << "Opened the simulation in your browser\n";
            else std::cout << "Could not launch a browser; open " << htmlPath << " manually\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
