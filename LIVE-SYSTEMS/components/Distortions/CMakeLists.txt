# Distortion Components
cmake_minimum_required(VERSION 3.15)

# Create distortion components library
add_library(LIVE-SYSTEMS-Distortions
    DistortionBase.cpp
    WaveshapeDistortion.cpp
)

target_include_directories(LIVE-SYSTEMS-Distortions
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(LIVE-SYSTEMS-Distortions
    PUBLIC
        juce::juce_core
        juce::juce_audio_basics
        juce::juce_dsp
)

target_compile_features(LIVE-SYSTEMS-Distortions PUBLIC cxx_std_20)