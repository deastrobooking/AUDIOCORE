# LIVE-SYSTEMS Audio Effects Library
cmake_minimum_required(VERSION 3.15)

# Add audio effects subdirectories
add_subdirectory(Delays)
add_subdirectory(Reverbs)
add_subdirectory(Chorus)
add_subdirectory(Compressor)

# Create a combined audio effects library
add_library(LIVE-SYSTEMS-AudioEffects INTERFACE)

target_link_libraries(LIVE-SYSTEMS-AudioEffects INTERFACE
    LIVE-SYSTEMS-Delays
    LIVE-SYSTEMS-Reverbs
    LIVE-SYSTEMS-Chorus
    LIVE-SYSTEMS-Compressor
)

# Export the library
target_include_directories(LIVE-SYSTEMS-AudioEffects INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)