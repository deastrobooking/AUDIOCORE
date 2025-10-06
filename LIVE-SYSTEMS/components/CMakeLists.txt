# LIVE-SYSTEMS Components Library
cmake_minimum_required(VERSION 3.15)

# Add component subdirectories
add_subdirectory(Diodes)
add_subdirectory(Resistors)
add_subdirectory(Filters)
add_subdirectory(Distortions)
add_subdirectory(Amplifiers)
add_subdirectory(Oscillators)

# Create a combined components library
add_library(LIVE-SYSTEMS-Components INTERFACE)

target_link_libraries(LIVE-SYSTEMS-Components INTERFACE
    LIVE-SYSTEMS-Diodes
    LIVE-SYSTEMS-Resistors
    LIVE-SYSTEMS-Filters
    LIVE-SYSTEMS-Distortions
    LIVE-SYSTEMS-Amplifiers
    LIVE-SYSTEMS-Oscillators
)

# Export the library
target_include_directories(LIVE-SYSTEMS-Components INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)