# Filters Components
cmake_minimum_required(VERSION 3.15)

add_library(LIVE-SYSTEMS-Filters INTERFACE)

# TODO: Add filter implementations
# add_library(LIVE-SYSTEMS-Filters
#     FilterBase.cpp
#     LowPassFilter.cpp
#     HighPassFilter.cpp
#     BandPassFilter.cpp
#     StateVariableFilter.cpp
# )

target_include_directories(LIVE-SYSTEMS-Filters INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(LIVE-SYSTEMS-Filters INTERFACE
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_dsp
)

target_compile_features(LIVE-SYSTEMS-Filters INTERFACE cxx_std_20)