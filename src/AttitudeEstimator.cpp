#include "AttitudeEstimator.h"

#include <cmath>

namespace attitude {

namespace {
constexpr double kPi = 3.14159265358979323846;
}

AttitudeEstimator::AttitudeEstimator(Convention convention)
    : convention_(convention) {}

double AttitudeEstimator::radToDeg(double rad) {
    return rad * (180.0 / kPi);
}

Attitude AttitudeEstimator::estimate(const AccelSample& a) const {
    Attitude out;

    switch (convention_) {
    case Convention::Legacy:
        out.pitchDeg = radToDeg(std::atan2(-a.y, std::sqrt(a.x * a.x + a.z * a.z)));
        out.rollDeg  = radToDeg(std::atan2(a.y, a.z));
        break;

    case Convention::AN3461_XYZ:
        out.rollDeg  = radToDeg(std::atan2(a.y, a.z));
        out.pitchDeg = radToDeg(std::atan2(-a.x, std::sqrt(a.y * a.y + a.z * a.z)));
        break;
    }

    return out;
}

} // namespace attitude
