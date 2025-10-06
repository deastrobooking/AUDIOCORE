# Simple Distortion Plugin Example

juce_add_plugin(SimpleDistortion
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    COMPANY_NAME "LIVE-SYSTEMS"                 # Specify the name of the plugin's author
    IS_SYNTH FALSE                              # Is this a synth or an effect?
    NEEDS_MIDI_INPUT FALSE                      # Does the plugin need midi input?
    NEEDS_MIDI_OUTPUT FALSE                     # Does the plugin need midi output?
    IS_MIDI_EFFECT FALSE                        # Is this plugin a MIDI effect?
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE           # Does the editor need keyboard focus?
    COPY_PLUGIN_AFTER_BUILD TRUE                # Should the plugin be installed to a default location after building?
    PLUGIN_MANUFACTURER_CODE Live               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE Dist                            # A unique four-character plugin id with exactly one upper-case character
                                                # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    FORMATS AU VST3 Standalone                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME "Simple Distortion"            # The name of the final executable, which can differ from the target name
)

# Generate JuceHeader.h and add source files
juce_generate_juce_header(SimpleDistortion)

target_sources(SimpleDistortion
    PRIVATE
        SimpleDistortionProcessor.cpp
        SimpleDistortionEditor.cpp
)

# Link with the LIVE-SYSTEMS framework and JUCE modules
target_link_libraries(SimpleDistortion
    PRIVATE
        LIVE-SYSTEMS-Framework
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Set C++ standard
target_compile_features(SimpleDistortion PRIVATE cxx_std_20)