# Wavetable Synthesizers
cmake_minimum_required(VERSION 3.15)

add_library(LIVE-SYSTEMS-Wavetable INTERFACE)

target_include_directories(LIVE-SYSTEMS-Wavetable INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(LIVE-SYSTEMS-Wavetable INTERFACE
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_dsp
)

target_compile_features(LIVE-SYSTEMS-Wavetable INTERFACE cxx_std_20)