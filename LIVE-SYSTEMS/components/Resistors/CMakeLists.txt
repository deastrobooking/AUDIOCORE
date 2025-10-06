# Resistors Components
cmake_minimum_required(VERSION 3.15)

add_library(LIVE-SYSTEMS-Resistors INTERFACE)

target_include_directories(LIVE-SYSTEMS-Resistors INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(LIVE-SYSTEMS-Resistors INTERFACE
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_dsp
)

target_compile_features(LIVE-SYSTEMS-Resistors INTERFACE cxx_std_20)