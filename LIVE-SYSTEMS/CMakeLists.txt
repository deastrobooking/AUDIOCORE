# CMakeList.txt : CMake project for LIVE-SYSTEMS Audio Plugin Framework

# Add component libraries
add_subdirectory("components")
add_subdirectory("audio_effects")
add_subdirectory("synths")

# Create the main framework library
add_library(LIVE-SYSTEMS-Framework STATIC
    "src/PluginProcessor.cpp"
    "src/PluginEditor.cpp"
    "src/AudioEngine.cpp"
    "src/ParameterManager.cpp"
    "src/PresetManager.cpp"
)

# Set C++ standard for the framework
target_compile_features(LIVE-SYSTEMS-Framework PRIVATE cxx_std_20)

# Enable Position Independent Code (required for plugins)
set_target_properties(LIVE-SYSTEMS-Framework PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Link with JUCE modules and component libraries
target_link_libraries(LIVE-SYSTEMS-Framework
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_dsp
        # Component libraries
        LIVE-SYSTEMS-Components
        LIVE-SYSTEMS-AudioEffects
        LIVE-SYSTEMS-Synths
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Include directories
target_include_directories(LIVE-SYSTEMS-Framework
    PUBLIC
        "include"
        "components"
        "audio_effects"
        "synths"
    PRIVATE
        "src"
)

# Compile definitions
target_compile_definitions(LIVE-SYSTEMS-Framework
    PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0
        JUCE_STRICT_REFCOUNTEDPOINTER=1
)

# Create example plugins that use the framework (optional)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/CMakeLists.txt")
    add_subdirectory("examples")
endif()
