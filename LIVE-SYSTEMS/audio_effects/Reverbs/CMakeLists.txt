# Reverbs Audio Effects
cmake_minimum_required(VERSION 3.15)

add_library(LIVE-SYSTEMS-Reverbs INTERFACE)

target_include_directories(LIVE-SYSTEMS-Reverbs INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(LIVE-SYSTEMS-Reverbs INTERFACE
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_dsp
)

target_compile_features(LIVE-SYSTEMS-Reverbs INTERFACE cxx_std_20)