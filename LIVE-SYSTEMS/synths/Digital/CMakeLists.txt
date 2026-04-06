# Digital Synthesizers
cmake_minimum_required(VERSION 3.15)

add_library(LIVE-SYSTEMS-Digital STATIC
    TwelveOperatorSynth.cpp
)

target_include_directories(LIVE-SYSTEMS-Digital
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(LIVE-SYSTEMS-Digital
    PUBLIC
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_dsp
)

target_compile_features(LIVE-SYSTEMS-Digital PUBLIC cxx_std_20)

set_target_properties(LIVE-SYSTEMS-Digital PROPERTIES POSITION_INDEPENDENT_CODE ON)