/**
 * Dependency-free unit tests for AttitudeEstimator.
 *
 * Each named case is registered individually in CMakeLists.txt so that
 * `ctest` reports them one by one.
 *
 *   test_attitude_estimator            run every case
 *   test_attitude_estimator <name>     run one case (used by ctest)
 *   test_attitude_estimator --list     print case names
 *
 * Exit code is non-zero if any assertion fails or the case name is unknown.
 */
#include "AttitudeEstimator.h"

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

using namespace attitude;

namespace {

int failures = 0;

void expectNear(const char* label, double actual, double expected, double tol = 1e-3) {
    if (std::fabs(actual - expected) > tol) {
        std::cerr << "  FAIL " << label << ": expected " << expected
                  << ", got " << actual << "\n";
        ++failures;
    } else {
        std::cout << "  ok   " << label << "\n";
    }
}

const AttitudeEstimator legacy(AttitudeEstimator::Convention::Legacy);
const AttitudeEstimator an3461(AttitudeEstimator::Convention::AN3461_XYZ);

struct TestCase {
    const char* name;
    std::function<void()> run;
};

const std::vector<TestCase> kCases = {
    {"legacy_regression_first_row", [] {
        // First row of attitude_exam.log must match the original program's output.
        Attitude a = legacy.estimate({27, -22, -982});
        expectNear("pitch", a.pitchDeg, 1.2829, 5e-5);
        expectNear("roll", a.rollDeg, -178.7166, 5e-5);
    }},
    {"level_sensor_gives_zero_angles", [] {
        expectNear("legacy pitch", legacy.estimate({0, 0, 1000}).pitchDeg, 0.0);
        expectNear("legacy roll", legacy.estimate({0, 0, 1000}).rollDeg, 0.0);
        expectNear("AN3461 pitch", an3461.estimate({0, 0, 1000}).pitchDeg, 0.0);
        expectNear("AN3461 roll", an3461.estimate({0, 0, 1000}).rollDeg, 0.0);
    }},
    {"scale_invariance", [] {
        // mg vs g vs m/s^2 must give identical angles: only ratios matter.
        Attitude mg = an3461.estimate({300, -200, 900});
        Attitude g = an3461.estimate({0.3, -0.2, 0.9});
        expectNear("pitch", mg.pitchDeg, g.pitchDeg, 1e-9);
        expectNear("roll", mg.rollDeg, g.rollDeg, 1e-9);
    }},
    {"an3461_roll_45deg", [] {
        expectNear("roll", an3461.estimate({0, 707, 707}).rollDeg, 45.0, 0.01);
        expectNear("pitch stays 0", an3461.estimate({0, 707, 707}).pitchDeg, 0.0, 0.01);
    }},
    {"an3461_pitch_signs", [] {
        // Gravity fully along +X -> pitch -90 (eq. 26 sign convention).
        expectNear("pitch -90", an3461.estimate({1000, 0, 0}).pitchDeg, -90.0, 0.01);
        expectNear("pitch +45", an3461.estimate({-707, 0, 707}).pitchDeg, 45.0, 0.01);
    }},
    {"legacy_pitch_ignores_x_axis", [] {
        // Documents the legacy quirk: X tilt does NOT change legacy pitch,
        // whereas the AN3461 equations respond to it.
        expectNear("legacy pitch", legacy.estimate({1000, 0, 0}).pitchDeg, 0.0);
        expectNear("AN3461 pitch", an3461.estimate({1000, 0, 0}).pitchDeg, -90.0, 0.01);
    }},
    {"upside_down_sensor_reads_180_roll", [] {
        expectNear("|roll|", std::fabs(an3461.estimate({0, 0, -1000}).rollDeg), 180.0, 0.01);
    }},
    {"rad_to_deg", [] {
        expectNear("pi -> 180", AttitudeEstimator::radToDeg(M_PI), 180.0, 1e-9);
    }},
};

int runCase(const TestCase& tc) {
    std::cout << "[" << tc.name << "]\n";
    const int before = failures;
    tc.run();
    return failures - before;
}

} // namespace

int main(int argc, char** argv) {
    if (argc > 1 && std::strcmp(argv[1], "--list") == 0) {
        for (const auto& tc : kCases) std::cout << tc.name << "\n";
        return EXIT_SUCCESS;
    }

    if (argc > 1) {
        for (const auto& tc : kCases) {
            if (std::strcmp(argv[1], tc.name) == 0) {
                return runCase(tc) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
            }
        }
        std::cerr << "Unknown test case: " << argv[1] << " (see --list)\n";
        return EXIT_FAILURE;
    }

    for (const auto& tc : kCases) runCase(tc);
    if (failures) {
        std::cerr << failures << " assertion(s) failed\n";
        return EXIT_FAILURE;
    }
    std::cout << "All " << kCases.size() << " cases passed\n";
    return EXIT_SUCCESS;
}
