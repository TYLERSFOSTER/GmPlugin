# ===================== Tests/CMakeLists.txt =====================
include(FetchContent)
FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.2
)
FetchContent_MakeAvailable(catch2)

# Pick up all test .cpp files
file(GLOB_RECURSE TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/dsp_tests/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/params_tests/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/plugin_tests/*.cpp"
)

add_executable(GmPlugin_tests
    ${TEST_SOURCES}

    # 🔴 Crucial: also compile the processor implementation into the test exe
    ../Source/plugin/PluginProcessor.cpp
)

# Includes for your headers
target_include_directories(GmPlugin_tests PRIVATE ../Source)

# Link Catch2 + JUCE modules used by PluginProcessor.cpp
target_link_libraries(GmPlugin_tests PRIVATE
    Catch2::Catch2WithMain
    juce::juce_audio_processors
    juce::juce_audio_basics
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_gui_basics
    juce::juce_gui_extra
)

# (Optional) same tiny defs you used on the plugin target
target_compile_definitions(GmPlugin_tests PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)

# Proper CTest + Catch2 integration
enable_testing()
include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)

catch_discover_tests(GmPlugin_tests)
