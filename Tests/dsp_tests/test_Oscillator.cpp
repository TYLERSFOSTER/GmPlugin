// ===================== Tests/dsp_tests/test_Oscillator.cpp =====================
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <cmath>
#include "../../Source/dsp/Oscillator.h"
#include "../../Source/params/KnobState.h"

// Helper to make a fresh oscillator
static Oscillator makeOsc(float sr = 48000.0f) {
    Oscillator o;
    o.prepare(sr);
    return o;
}

// Convenience wrapper to call osc.process() with values from a KnobState
static float stepOsc(Oscillator& osc, const KnobState& k) {
    return osc.process(
        k.lambda_hz.load(),
        k.phase_offset_rad.load(),
        k.A_dB.load(),
        k.u_r_mag.load(),
        k.u_phi_rad.load()
    );
}

// =========================================================
// 1. Deterministic math checks
// =========================================================

TEST_CASE("Oscillator: phase advances correctly per sample", "[oscillator]") {
    KnobState k;
    k.lambda_hz.store(1000.0f); // 1 kHz
    Oscillator osc = makeOsc(48000.0f);

    float initial_phase = osc.phase;
    stepOsc(osc, k);
    float delta = osc.phase - initial_phase;
    float expected = 2 * M_PI * 1000.0f / 48000.0f;
    REQUIRE(std::fabs(delta - expected) < 1e-6f);
}

TEST_CASE("Oscillator: amplitude matches expected linear dB sum", "[oscillator]") {
    KnobState k;
    k.A_dB.store(6.0f);
    k.u_mag_dB.store(6.0f);
    k.u_r_mag.store(1.0f);    // neutral modulation
    k.u_phi_rad.store(0.0f);  // no complex rotation
    Oscillator osc = makeOsc();
    float out = stepOsc(osc, k);

    constexpr float LN10_OVER_20 = 0.115129254f;
    float expected_amp = std::exp((6.0f + 6.0f) * LN10_OVER_20);
    REQUIRE(std::fabs(std::fabs(out) - expected_amp) < 1e-6f);
}

// =========================================================
// 2. Stability / edge-case tests
// =========================================================

TEST_CASE("Oscillator: zero frequency stays constant", "[oscillator]") {
    KnobState k;
    k.lambda_hz.store(0.0f);
    Oscillator osc = makeOsc();
    float out1 = stepOsc(osc, k);
    float out2 = stepOsc(osc, k);
    REQUIRE(out1 == Catch::Approx(out2));
}

TEST_CASE("Oscillator: negative frequency reverses phase direction", "[oscillator]") {
    KnobState k;
    k.lambda_hz.store(-1000.0f);
    Oscillator osc = makeOsc(48000.0f);

    stepOsc(osc, k);
    float after_neg = osc.phase;
    REQUIRE(after_neg < 0.0f);  // phase should move backward
}

// =========================================================
// 3. Integration / state continuity
// =========================================================

TEST_CASE("Oscillator: prepare resets phase", "[oscillator]") {
    Oscillator osc = makeOsc();
    osc.phase = 3.14f;
    osc.prepare(48000.0f);
    REQUIRE(osc.phase == Catch::Approx(0.0f));
}

TEST_CASE("Oscillator: phase wraps cleanly under 2π", "[oscillator]") {
    KnobState k;
    k.lambda_hz.store(48000.0f);  // 1 cycle per sample rate
    Oscillator osc = makeOsc(48000.0f);

    for (int i = 0; i < 48000; ++i)
        stepOsc(osc, k);

    REQUIRE(osc.phase < 2 * M_PI);
    REQUIRE(osc.phase >= 0.0f);
}
