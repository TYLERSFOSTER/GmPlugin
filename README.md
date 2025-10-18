<p align="left">
  <picture>
    <source srcset="assets/U(1)_dark.jpg" media="(prefers-color-scheme: dark)">
    <source srcset="assets/U(1)_light.jpg" media="(prefers-color-scheme: light)">
    <img src="docs/images/U(1)_light.jpg" alt="tonnetzB" width="400">
  </picture>
</p>

# *MIDI-controlled* 𝔾ₘ-*modulator*

Audio plugin that implements a MIDI-controlled synth that modulates oscillators
$$
e^{z}\;=\;e^{\text{ln}A+i\,2\pi\,\lambda\,t}
$$
by performing complex argument multiplication with an arbitrary complex number $u$ that the user determines through MIDI-contoller knobs 1, 2, and 3.

## 🧩 Build

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --output-on-failure
```
After building, CMake (via JUCE) generates the following targets:
| Output Type         | Path                                               | Description                                                   |
| ------------------- | -------------------------------------------------- | ------------------------------------------------------------- |
| **Standalone App**  | `build/GmPlugin_artefacts/Standalone/GmPlugin.app` | Run directly from Terminal for testing and live MIDI control  |
| **VST3 Plugin**     | `build/GmPlugin_artefacts/VST3/GmPlugin.vst3`      | Standard plugin for DAWs like FL Studio, Ableton Live, Reaper |
| **Audio Unit (AU)** | `build/GmPlugin_artefacts/AU/GmPlugin.component`   | macOS Audio Unit plugin for Logic Pro and GarageBand          |
| **Unit Tests**      | `build/Tests/`                                     | Compiled test binaries, runnable via `ctest`                  |

All generated binaries and plugin bundles are self-contained inside `build/GmPlugin_artefacts/`.

## 🖥️ Run from Terminal (Standalone Build)

To run the plugin as a standalone synthesizer (outside any DAW):

```bash
./build/GmPlugin_artefacts/Standalone/GmPlugin.app/Contents/MacOS/GmPlugin
```

You should then open **Options → Audio/MIDI Settings** in the GUI window. Enable your MIDI controller (e.g. MPK Mini Mk II), and then twist knobs or press keys to see live MIDI messages in the console. You should hear the resulting sound synthesis directly.

## 🎚️ Use in FL Studio / Other DAWs

Once built, JUCE generates plugin binaries automatically:

| Plugin Type         | macOS Path                                               |
| ------------------- | -------------------------------------------------------- |
| **VST3**            | `~/Library/Audio/Plug-Ins/VST3/GmPlugin.vst3`            |
| **Audio Unit (AU)** | `~/Library/Audio/Plug-Ins/Components/GmPlugin.component` |

### In FL Studio (macOS):

1. Open Plugin Manager (Options → Manage Plugins).
2. Click Find Plugins to rescan.
3. Make sure GmPlugin appears and is enabled (green check). It will show up under **Add → More Plugins → Generators → GmPlugin**.

### In Ableton Live / Logic / Reaper:
The plugin appears automatically after a rescan in the corresponding VST3 or AU list.


## 📂 Project Layout

```bash
.
├── assets
├── CMakeLists.txt
├── LICENSE
├── README.md
├── Source
│   ├── dsp
│   │   └── Oscillator.h
│   ├── params
│   │   ├── KnobState.h
│   │   ├── ParameterIDs.h
│   │   └── ParamSpec.h
│   └── plugin
│       ├── PluginEditor.cpp
│       ├── PluginEditor.h
│       ├── PluginProcessor.cpp
│       └── PluginProcessor.h
├── Testing
└── Tests
```

## 🧪 Testing

Run all tests with:
```bash
ctest --output-on-failure
```

## ⚙️ Dependencies

- C++20
- CMake ≥ 3.20
- [Catch2](https://github.com/catchorg/Catch2) for testing

## 📜 License

This project is licensed under the [MIT License](LICENSE.txt).
