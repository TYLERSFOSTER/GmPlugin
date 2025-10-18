// ===================== test_KnobState.cpp =====================
#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "../Source/params/KnobState.h"
#include <cmath>

// Helper for floating-point comparisons
static constexpr float EPS = 1e-5f;

TEST_CASE("KnobState::totalAmpLinear converts dB to linear correctly") {
    KnobState ks;
    ks.A_dB.store(6.0f);
    ks.u_mag_dB.store(0.0f);

    float expected = std::exp((6.0f + 0.0f) * (std::log(10.0f) / 20.0f));
    REQUIRE(std::fabs(ks.totalAmpLinear() - expected) < EPS);
}

TEST_CASE("KnobState::uPhase adds phase and offset") {
    KnobState ks;
    ks.u_theta_rad.store(1.0f);
    ks.phase_offset_rad.store(0.5f);
    REQUIRE(std::fabs(ks.uPhase() - 1.5f) < EPS);
}

TEST_CASE("KnobState::clippingEnabled behaves as boolean threshold") {
    KnobState ks;

    ks.clip_enable.store(0.4f);
    REQUIRE_FALSE(ks.clippingEnabled());

    ks.clip_enable.store(0.6f);
    REQUIRE(ks.clippingEnabled());
}

TEST_CASE("ParamSpec::logDomain toggles between dB and linear") {
    auto spec = PARAM_SPECS[(int)ParameterID::A_dB];
    REQUIRE(toLinear(spec, 0.0f) == Catch::Approx(1.0f));
    REQUIRE(toLinear(spec, 6.0f) == Catch::Approx(1.995f).epsilon(0.01)); // 6 dB ≈ 2× gain
}
