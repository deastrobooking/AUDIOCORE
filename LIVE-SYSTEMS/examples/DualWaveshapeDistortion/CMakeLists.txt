# Dual Waveshape Distortion VST3 Plugin
juce_add_plugin(DualWaveshapeDistortion
    COMPANY_NAME "AUDIOCORE"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Adcr
    PLUGIN_CODE Dwds
    FORMATS VST3 Standalone
    PRODUCT_NAME "Dual Waveshape Distortion"
    VST3_CATEGORIES Fx Distortion
    AU_MAIN_TYPE kAudioUnitType_Effect
)

target_sources(DualWaveshapeDistortion
    PRIVATE
        DualWaveshapeProcessor.cpp
        DualWaveshapeProcessor.h
        DualWaveshapeEditor.cpp
        DualWaveshapeEditor.h
)

target_compile_definitions(DualWaveshapeDistortion
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(DualWaveshapeDistortion
    PRIVATE
        LIVE-SYSTEMS-Framework
        LIVE-SYSTEMS-Distortions
        LiveSystems_Oscillators
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

set_target_properties(DualWaveshapeDistortion PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
