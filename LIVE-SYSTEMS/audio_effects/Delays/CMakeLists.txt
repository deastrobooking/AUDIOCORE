# Delays Audio Effects
cmake_minimum_required(VERSION 3.15)

add_library(LIVE-SYSTEMS-Delays INTERFACE)

# TODO: Add delay implementations
# add_library(LIVE-SYSTEMS-Delays
#     DelayBase.cpp
#     SimpleDelay.cpp
#     PingPongDelay.cpp
#     TapDelay.cpp
# )

target_include_directories(LIVE-SYSTEMS-Delays INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(LIVE-SYSTEMS-Delays INTERFACE
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_dsp
)

target_compile_features(LIVE-SYSTEMS-Delays INTERFACE cxx_std_20)