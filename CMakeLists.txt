# ===================== CMakeLists.txt =====================
cmake_minimum_required(VERSION 3.20)
project(GmPlugin VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------------------------------------
# JUCE setup
# ----------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 7.0.9
)
FetchContent_MakeAvailable(juce)

# ----------------------------------------------------------
# GmPlugin target
# ----------------------------------------------------------
juce_add_plugin(GmPlugin
    VERSION                 ${PROJECT_VERSION}
    COMPANY_NAME            "TylerSFoster"
    IS_SYNTH                TRUE
    NEEDS_MIDI_INPUT        TRUE
    NEEDS_MIDI_OUTPUT       FALSE
    IS_MIDI_EFFECT          FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Tylr
    PLUGIN_CODE              Gmpl
    FORMATS                 AU VST3 Standalone
    PRODUCT_NAME            "GmPlugin"
)

# ----------------------------------------------------------
# JUCE modules
# ----------------------------------------------------------
target_link_libraries(GmPlugin
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_audio_basics
        juce::juce_dsp
        juce::juce_gui_basics
        juce::juce_gui_extra
)

# ----------------------------------------------------------
# Plugin source files
# ----------------------------------------------------------
target_sources(GmPlugin PRIVATE
    Source/plugin/PluginProcessor.cpp
    Source/plugin/PluginProcessor.h
    Source/plugin/PluginEditor.cpp
    Source/plugin/PluginEditor.h
)

target_include_directories(GmPlugin PRIVATE
    Source
)

target_compile_definitions(GmPlugin
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0

        # prevent any legacy VST2 code from being included
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_PLUGINHOST_VST=0
        JUCE_PLUGINHOST_VST3=0
)

# ----------------------------------------------------------
# Unit tests (Catch2)
# ----------------------------------------------------------
include(CTest)
enable_testing()  # ← ADD THIS LINE

if (BUILD_TESTING)
    add_subdirectory(Tests)
endif()

