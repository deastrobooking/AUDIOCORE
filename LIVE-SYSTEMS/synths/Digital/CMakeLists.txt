# Digital Synthesizers
cmake_minimum_required(VERSION 3.15)

add_library(LIVE-SYSTEMS-Digital INTERFACE)

target_include_directories(LIVE-SYSTEMS-Digital INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(LIVE-SYSTEMS-Digital INTERFACE
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_dsp
)

target_compile_features(LIVE-SYSTEMS-Digital INTERFACE cxx_std_20)