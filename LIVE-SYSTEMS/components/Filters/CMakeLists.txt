# Filters Components
cmake_minimum_required(VERSION 3.15)

add_library(LIVE-SYSTEMS-Filters
    FilterBase.cpp
    LowPassFilter.cpp
    HighPassFilter.cpp
    BandPassFilter.cpp
)

target_include_directories(LIVE-SYSTEMS-Filters
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(LIVE-SYSTEMS-Filters
    PUBLIC
        juce::juce_core
        juce::juce_audio_basics
        juce::juce_dsp
)

target_compile_features(LIVE-SYSTEMS-Filters PUBLIC cxx_std_20)

set_target_properties(LIVE-SYSTEMS-Filters PROPERTIES POSITION_INDEPENDENT_CODE ON)