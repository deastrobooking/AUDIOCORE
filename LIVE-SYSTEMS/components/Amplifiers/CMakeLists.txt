# Amplifiers Components
cmake_minimum_required(VERSION 3.15)

add_library(LIVE-SYSTEMS-Amplifiers INTERFACE)

target_include_directories(LIVE-SYSTEMS-Amplifiers INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(LIVE-SYSTEMS-Amplifiers INTERFACE
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_dsp
)

target_compile_features(LIVE-SYSTEMS-Amplifiers INTERFACE cxx_std_20)