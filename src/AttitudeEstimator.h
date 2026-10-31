#pragma once

/**
 * AttitudeEstimator — pure pitch/roll math from a 3-axis accelerometer.
 *
 * This class is deliberately free of any I/O, allocation or file-format
 * knowledge so it can be:
 *   - unit-tested in isolation,
 *   - reused by other front-ends (CLI, WebAssembly, tests),
 *   - ported 1:1 to another language (e.g. TypeScript for the browser sim).
 *
 * Units: the accelerometer components may be in any consistent unit
 * (mg, g, m/s^2). Only ratios between axes are used, so scale cancels out.
 * Output angles are in degrees.
 */
namespace attitude {

struct AccelSample {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
};

struct Attitude {
    double pitchDeg = 0.0;
    double rollDeg = 0.0;
};

class AttitudeEstimator {
public:
    /**
     * Which set of equations to use.
     *
     * Legacy     — reproduces the original main.cpp bit-for-bit in spirit:
     *                pitch = atan2(-ay, sqrt(ax^2 + az^2))
     *                roll  = atan2( ay, az)
     *              NOTE: both angles depend on ay and ax is only used inside
     *              the sqrt. This does not match any single rotation sequence
     *              in NXP AN3461 and is kept only for backward compatibility.
     *
     * AN3461_XYZ — NXP AN3461 eq. 25 / 26 (roll about X, then pitch about Y):
     *                roll  = atan2( ay, az)
     *                pitch = atan2(-ax, sqrt(ay^2 + az^2))
     *              This is the textbook formulation and is the recommended
     *              default for new work.
     */
    enum class Convention {
        Legacy,
        AN3461_XYZ,
    };

    explicit AttitudeEstimator(Convention convention = Convention::Legacy);

    /** Estimate pitch and roll (degrees) for one accelerometer sample. */
    Attitude estimate(const AccelSample& accel) const;

    Convention convention() const { return convention_; }

    static double radToDeg(double rad);

private:
    Convention convention_;
};

} // namespace attitude
