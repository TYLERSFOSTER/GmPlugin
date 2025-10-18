#include "PluginProcessor.h"
#include <juce_audio_devices/juce_audio_devices.h>  // for MidiDeviceInfo
#include <unordered_map>
#include <iostream>

// ============================================================
// MIDI CC Definitions — explicit (fast) + introspectable (future-ready)
// Use ACTUAL controller numbers. Your logs showed CC #4 and CC #5.
// ============================================================
enum MidiCC {
    CC_Amplitude = 1,  // Mod wheel (often)
    CC_Frequency = 2,
    CC_Phase     = 3,
    CC_uR        = 4,  // r magnitude (Foot Controller / CC4)
    CC_uPhi      = 5   // φ rotation  (Portamento Time / CC5)
};

static const std::unordered_map<int, const char*> ccToParamID = {
    { CC_Amplitude, "A_dB" },
    { CC_Frequency, "lambda_hz" },
    { CC_Phase,     "phase_offset_rad" },
    { CC_uR,        "u_r_mag" },
    { CC_uPhi,      "u_phi_rad" }
};

// ============================================================
// GmPluginAudioProcessor implementation
// ============================================================

GmPluginAudioProcessor::GmPluginAudioProcessor()
{
    // Print available MIDI inputs at startup (no “auto-enable” here –
    // use Options -> Audio/MIDI Settings in the Standalone app)
    const auto devices = juce::MidiInput::getAvailableDevices();

    std::cout << "=== Available MIDI inputs ===\n";
    if (devices.size() == 0)
    {
        std::cout << "(none)\n";
    }
    else
    {
        for (const auto& d : devices)
            std::cout << "• " << d.identifier.toStdString()
                      << " — " << d.name.toStdString() << "\n";
    }
    std::cout << "=============================\n" << std::flush;

    DBG("[GmPlugin] MIDI input list printed to stdout.");
}

GmPluginAudioProcessor::~GmPluginAudioProcessor() {}

void GmPluginAudioProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    sampleRateHz = static_cast<float>(sampleRate);

    // Prepare any already-existing voices (usually none on first call)
    for (auto& [note, v] : voices)
        v.osc.prepare(sampleRateHz);
}

void GmPluginAudioProcessor::releaseResources() {}

void GmPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                          juce::MidiBuffer& midiMessages)
{
    // ====================================================
    // Handle incoming MIDI (create/destroy voices + CCs)
    // ====================================================
    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();

        // --- Log every message (both DBG and stdout) ---
        const auto desc = msg.getDescription();
        DBG("Raw MIDI: " << desc);
        std::cout << "[MIDI] " << desc.toStdString() << std::endl;

        if (msg.isController())
        {
            DBG("🧭 CC# " << msg.getControllerNumber()
                << "  value=" << msg.getControllerValue());
            std::cout << "[CC] #" << msg.getControllerNumber()
                      << " value=" << msg.getControllerValue() << std::endl;
        }
        else if (msg.isPitchWheel())
        {
            DBG("🎛️ Pitch Wheel value → " << msg.getPitchWheelValue());
            std::cout << "[PitchWheel] " << msg.getPitchWheelValue() << std::endl;
        }

        // ----------------------------------------------------
        // NOTE ON
        // ----------------------------------------------------
        if (msg.isNoteOn())
        {
            const int note = msg.getNoteNumber();
            const float semisFromA4 = static_cast<float>(note - 69);
            const float ratio = std::pow(2.0f, semisFromA4 / 12.0f);

            Voice v;
            v.ratio = ratio;
            v.osc.prepare(sampleRateHz);
            voices[note] = std::move(v);

            DBG("🎹 NOTE ON → " << note << "  voices=" << (int)voices.size());
            std::cout << "[NOTE ON] " << note << "  voices=" << voices.size() << std::endl;
        }
        // ----------------------------------------------------
        // NOTE OFF
        // ----------------------------------------------------
        else if (msg.isNoteOff())
        {
            const int note = msg.getNoteNumber();
            voices.erase(note);
            DBG("🎹 NOTE OFF → " << note << "  voices=" << (int)voices.size());
            std::cout << "[NOTE OFF] " << note << "  voices=" << voices.size() << std::endl;
        }
        // ----------------------------------------------------
        // MIDI CC messages (mapped parameters, GLOBAL)
        // ----------------------------------------------------
        else if (msg.isController())
        {
            const int cc = msg.getControllerNumber();
            const float norm = msg.getControllerValue() / 127.0f; // 0..1

            if (auto it = ccToParamID.find(cc); it != ccToParamID.end())
                DBG("MIDI CC: " << cc << " (" << it->second << ") value: " << norm);
            else
                DBG("MIDI CC: " << cc << " (unmapped) value: " << norm);

            switch (cc)
            {
                case CC_Amplitude:
                    // Map 0..1 → -24..+24 dB
                    knobState.A_dB = (norm * 48.0f) - 24.0f;
                    break;

                case CC_Frequency:
                    // Map 0..1 → 20..20000 Hz logarithmically
                    knobState.lambda_hz = 20.0f * std::pow(1000.0f, norm);  // 20–20k
                    break;

                case CC_Phase:
                    knobState.phase_offset_rad = norm * juce::MathConstants<float>::twoPi;
                    break;

                case CC_uR:
                    // r in [0.5 .. 2.0] as an example range
                    knobState.u_r_mag = 0.5f + norm * 1.5f;
                    break;

                case CC_uPhi:
                    // φ in [0 .. 2π]
                    knobState.u_phi_rad = norm * juce::MathConstants<float>::twoPi;
                    break;

                default:
                    break;
            }

            DBG("→ Updated knobState: "
                << "A_dB=" << knobState.A_dB
                << ", λ_hz=" << knobState.lambda_hz
                << ", phase=" << knobState.phase_offset_rad
                << ", u_r=" << knobState.u_r_mag
                << ", u_phi=" << knobState.u_phi_rad);
        }
    }

    // ====================================================
    // Audio generation (sum all active voices)
    // ====================================================
    const int numSamples  = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Clear output first
    for (int ch = 0; ch < numChannels; ++ch)
        buffer.clear(ch, 0, numSamples);

    // Global knobs applied to ALL voices:
    const float baseHz   = knobState.lambda_hz.load();
    const float phaseOff = knobState.phase_offset_rad.load();
    const float AdB      = knobState.A_dB.load();

    // 🔹 New complex modulation controls
    const float uR   = knobState.u_r_mag.load();
    const float uPhi = knobState.u_phi_rad.load();

    // Render
    for (int i = 0; i < numSamples; ++i)
    {
        float mixSample = 0.0f;

        for (auto& [note, v] : voices)
        {
            const float voiceHz = baseHz * v.ratio; // per-voice pitch from global base
            mixSample += v.osc.process(voiceHz, phaseOff, AdB, uR, uPhi);
        }

        // write to all channels (mono -> stereo, etc.)
        for (int ch = 0; ch < numChannels; ++ch)
            buffer.getWritePointer(ch)[i] = mixSample;
    }
}

// ============================================================
// Factory function JUCE requires
// ============================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GmPluginAudioProcessor();
}
