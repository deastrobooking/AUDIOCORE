# Diodes Components
cmake_minimum_required(VERSION 3.15)

add_library(LIVE-SYSTEMS-Diodes INTERFACE)

target_include_directories(LIVE-SYSTEMS-Diodes INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(LIVE-SYSTEMS-Diodes INTERFACE
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_dsp
)

target_compile_features(LIVE-SYSTEMS-Diodes INTERFACE cxx_std_20)