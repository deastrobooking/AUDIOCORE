# Oscillators components
add_library(LiveSystems_Oscillators STATIC
    LFO.cpp
    LFO.h
)

target_link_libraries(LiveSystems_Oscillators
    PUBLIC
        juce::juce_core
        juce::juce_audio_basics
        juce::juce_dsp
)

set_target_properties(LiveSystems_Oscillators PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# Create an alias for consistency with other components
add_library(LIVE-SYSTEMS-Oscillators ALIAS LiveSystems_Oscillators)
