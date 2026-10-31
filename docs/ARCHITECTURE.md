# Architecture

## Goal

One executable that turns an accelerometer log into pitch/roll angles, a CSV,
and a browser simulation of an airplane following those angles — with nothing
for the student to install or start.

## Design

```
┌──────────── pitch_roll (C++) ────────────────────────────────────────────┐
│ AccelLogReader → AttitudeEstimator → EstimationWriter → output_estimations.log
│                          │
│                          └→ SimulationPage → output_simulation.html → Browser
└───────────────────────────────────────────────────────────────────────────┘
                                                     │
                           web/simulation.html (embedded in the binary at build time)
                           data inlined as JSON  →  opens from disk, no server
```

Principles:

1. **The math lives in one place.** `AttitudeEstimator` is a small class with
   no I/O. Everything else is plumbing around it. The browser does *not*
   re-implement the equations; it animates the numbers the C++ produced.
2. **No runtime dependencies.** The page is plain HTML + Canvas 2D. A single
   `<script>` in `web/simulation.html` is organised top-to-bottom as: data →
   rotation math → playback → airplane mesh → 3D scene → horizon → charts →
   formula panel → wiring. It is meant to be read.
3. **No servers.** ES modules and `fetch()` do not work from `file://`, so the
   page is one file with the data inlined. CMake embeds the template in the
   binary as a raw string literal (`build/generated/WebAssets.h`); the program
   replaces three placeholders (`__SOURCE_NAME__`, `__EQUATIONS__`, `__DATA__`)
   and launches the system browser.

## Orientation from angles

Frames: x forward (nose), y left wing, z up. AN3461 gives the body-frame
gravity reaction as `g_b = [−sin θ, cos θ sin φ, cos θ cos φ]`, and the matrix
mapping body to world is **`R = Ry(θ)·Rx(φ)`**, which sends `g_b` to `[0, 0, 1]`.
The airplane mesh is transformed by `R`; the measured acceleration vector is
also transformed by `R` and drawn — if the equations match the data it is
vertical. This is the built-in correctness check shown in the HUD.

Display corrections, deliberately visible in the page's JavaScript:

- *Upside-down sensor*: `(x, y, z) → (x, −y, −z)` ⇒ roll + 180°, pitch unchanged.
- *Artificial horizon*: aviation shows nose-up as positive; in the AN3461 frame
  nose-up gives negative pitch, so the instrument negates pitch for display.

## Known limitations / open questions

- The mounting correction assumes a 180° rotation about **X**. A rotation about
  **Y** also fits the data (`az` becomes +1 g) but flips the sign of pitch. Only
  the physical setup can decide.
- Yaw is unobservable from an accelerometer alone; the page says so.
- Accelerometer-only estimates are noisy and respond to linear acceleration,
  not just gravity. Filtering / gyro fusion would be the natural next lesson.
- `std::system` is used to launch the browser; on a machine with no browser the
  program prints the path to open instead.
