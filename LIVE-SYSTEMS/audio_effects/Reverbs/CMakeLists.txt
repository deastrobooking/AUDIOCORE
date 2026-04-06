# Reverbs Audio Effects
cmake_minimum_required(VERSION 3.15)

add_library(LIVE-SYSTEMS-Reverbs STATIC
    Reverb.cpp
)

target_include_directories(LIVE-SYSTEMS-Reverbs
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(LIVE-SYSTEMS-Reverbs
    PUBLIC
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_dsp
)

target_compile_features(LIVE-SYSTEMS-Reverbs PUBLIC cxx_std_20)
set_target_properties(LIVE-SYSTEMS-Reverbs PROPERTIES POSITION_INDEPENDENT_CODE ON)
