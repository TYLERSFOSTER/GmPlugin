// ===================== ParamSpec.h =====================
#pragma once
#include "ParameterIDs.h"
#include <cmath>
#include <string>

// ============================================================
// ParamSpec
// ------------------------------------------------------------
// Describes each plugin parameter (metadata only).
// ============================================================
struct ParamSpec {
    ParameterID id;
    const char* name;
    float min;
    float max;
    float defaultValue;
    const char* unit;
    bool midiExposed;   // visible to host automation
    bool peripheral;    // hardware-mapped parameter
    bool logDomain;     // true if stored in log scale (e.g. dB)
};

// ============================================================
// Parameter table
// ------------------------------------------------------------
// Fields: id, name, min, max, default, unit,
//         midiExposed, peripheral, logDomain
// ============================================================
static constexpr ParamSpec PARAM_SPECS[] = {
    { ParameterID::u_mag_dB,        "u magnitude (dB)",  -24.0f,   +24.0f,   0.0f,  "dB",   true,  false,  true  },
    { ParameterID::u_theta,         "u angle",             0.0f,    6.28318f, 0.0f,  "rad",  true,  false,  false },
    { ParameterID::lambda_hz,       "lambda",              1.0f, 20000.0f, 440.0f,  "Hz",   true,  false,  true  },
    { ParameterID::A_dB,            "A (dB amp)",        -24.0f,   +24.0f,   0.0f,  "dB",   true,  false,  true  },
    { ParameterID::clip_enable,     "Clip enable",         0.0f,    1.0f,     1.0f,  "bool", true,  false,  false },
    { ParameterID::phase_offset_rad,"Phase offset",        0.0f,    6.28318f, 0.0f,  "rad",  true,  true,  false },
    { ParameterID::u_r_mag,         "u r magnitude",       0.0f,   10000.0f,    1.0f,  "scalar", true, true, false },
    { ParameterID::u_phi_rad,       "u phi angle",         0.0f,   6283.18f, 0.0f,  "rad",    true, true, false },
};

// ============================================================
// Conversion helper
// ------------------------------------------------------------
// Converts a parameter’s raw stored value to its linear equivalent.
// ============================================================
inline float toLinear(const ParamSpec& spec, float value)
{
    if (spec.logDomain)
        return std::pow(10.0f, value / 20.0f);  // convert dB → linear
    return value;                               // already linear
}
