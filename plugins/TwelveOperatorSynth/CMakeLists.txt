juce_add_plugin(TwelveOperatorSynth
    COMPANY_NAME            "LIVE-SYSTEMS"
    IS_SYNTH                TRUE
    NEEDS_MIDI_INPUT        TRUE
    NEEDS_MIDI_OUTPUT       FALSE
    IS_MIDI_EFFECT          FALSE
    PLUGIN_MANUFACTURER_CODE Live
    PLUGIN_CODE             TwOp
    FORMATS                 AU VST3 Standalone
    PRODUCT_NAME            "12-Op FM Synth"
)

juce_generate_juce_header(TwelveOperatorSynth)

target_sources(TwelveOperatorSynth
    PRIVATE
        TwelveOperatorSynthProcessor.cpp
        TwelveOperatorSynthEditor.cpp
)

target_include_directories(TwelveOperatorSynth
    PRIVATE
        ${CMAKE_SOURCE_DIR}/LIVE-SYSTEMS/include
        ${CMAKE_SOURCE_DIR}/LIVE-SYSTEMS/synths/Digital
        ${CMAKE_SOURCE_DIR}/LIVE-SYSTEMS/audio_effects/Chorus
        ${CMAKE_SOURCE_DIR}/LIVE-SYSTEMS/audio_effects/Delays
        ${CMAKE_SOURCE_DIR}/LIVE-SYSTEMS/audio_effects/Reverbs
        ${CMAKE_SOURCE_DIR}/LIVE-SYSTEMS/audio_effects/Compressor
)

target_link_libraries(TwelveOperatorSynth
    PRIVATE
        LIVE-SYSTEMS-Framework
        LIVE-SYSTEMS-Digital
        LIVE-SYSTEMS-AudioEffects
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

target_compile_features(TwelveOperatorSynth PRIVATE cxx_std_20)
