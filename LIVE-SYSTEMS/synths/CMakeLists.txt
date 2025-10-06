# LIVE-SYSTEMS Synthesizers Library
cmake_minimum_required(VERSION 3.15)

# Add synthesizer subdirectories
add_subdirectory(Analog)
add_subdirectory(Digital)
add_subdirectory(Wavetable)

# Create a combined synthesizers library
add_library(LIVE-SYSTEMS-Synths INTERFACE)

target_link_libraries(LIVE-SYSTEMS-Synths INTERFACE
    LIVE-SYSTEMS-Analog
    LIVE-SYSTEMS-Digital
    LIVE-SYSTEMS-Wavetable
)

# Export the library
target_include_directories(LIVE-SYSTEMS-Synths INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)