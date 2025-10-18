// ===================== KnobState.h =====================
#pragma once
#include "ParameterIDs.h"
#include "ParamSpec.h"
#include <atomic>
#include <cmath>

struct KnobState {
    // Core knob values
    std::atomic<float> u_mag_dB         { 0.0f };
    std::atomic<float> u_theta_rad      { 0.0f };
    std::atomic<float> lambda_hz        { 440.0f };
    std::atomic<float> A_dB             { 0.0f };
    std::atomic<float> clip_enable      { 1.0f };
    std::atomic<float> phase_offset_rad { 0.0f };

    // 🔹 NEW: complex modulation magnitude and phase
    std::atomic<float> u_r_mag          { 1.0f };  // scalar multiplier
    std::atomic<float> u_phi_rad        { 0.0f };  // radians, rotates modulation

    // Convenience getters
    inline float totalAmpLinear() const noexcept {
        const auto& A = PARAM_SPECS[(int)ParameterID::A_dB];
        const auto& u = PARAM_SPECS[(int)ParameterID::u_mag_dB];
        return toLinear(A, A_dB.load()) * toLinear(u, u_mag_dB.load());
    }

    inline float uPhase() const noexcept {
        return u_theta_rad.load() + phase_offset_rad.load();
    }

    inline bool clippingEnabled() const noexcept {
        return clip_enable.load() >= 0.5f;
    }
};
