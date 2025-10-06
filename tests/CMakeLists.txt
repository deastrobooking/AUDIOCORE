# Tests for LIVE-SYSTEMS Framework
cmake_minimum_required(VERSION 3.15)

# Create a test runner executable
add_executable(LiveSystemsTests
    test_main.cpp
)

target_link_libraries(LiveSystemsTests
    PRIVATE
        LIVE-SYSTEMS-Framework
        juce::juce_audio_basics
        juce::juce_core
)

target_compile_features(LiveSystemsTests PRIVATE cxx_std_20)

# Add the test executable to CTest
add_test(NAME LiveSystemsTests COMMAND LiveSystemsTests)
