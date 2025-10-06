# Oscillators Components
cmake_minimum_required(VERSION 3.15)

add_library(LIVE-SYSTEMS-Oscillators INTERFACE)

# TODO: Add oscillator implementations
# add_library(LIVE-SYSTEMS-Oscillators
#     OscillatorBase.cpp
#     SineOscillator.cpp
#     SawOscillator.cpp
#     SquareOscillator.cpp
#     WavetableOscillator.cpp
# )

target_include_directories(LIVE-SYSTEMS-Oscillators INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(LIVE-SYSTEMS-Oscillators INTERFACE
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_dsp
)

target_compile_features(LIVE-SYSTEMS-Oscillators INTERFACE cxx_std_20)