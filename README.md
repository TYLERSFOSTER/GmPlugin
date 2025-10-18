<p align="left">
  <picture>
    <source srcset="assets/U(1)_dark.jpg" media="(prefers-color-scheme: dark)">
    <source srcset="assets/U(1)_light.jpg" media="(prefers-color-scheme: light)">
    <img src="docs/images/U(1)_light.jpg" alt="tonnetzB" width="400">
  </picture>
</p>

# *MIDI-controlled* 𝔾ₘ-*modulator*

A research-grade audio plugin experiment written in modern C++20.
Implements real-time signal generation from complex-valued control parameters.

## 🧩 Build

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --output-on-failure
```

## 📂 Project Layout

```bash
.
├── assets
├── CMakeLists.txt
├── LICENSE.txt
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
└── Tests
    ├── CMakeLists.txt
    └── dsp_tests
        ├── test_KnobState.cpp
        └── test_Oscillator.cpp
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
