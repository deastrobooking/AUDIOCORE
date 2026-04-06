#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "TwelveOperatorSynthProcessor.h"

//==============================================================================
class TwelveOperatorSynthEditor : public juce::AudioProcessorEditor
{
public:
    explicit TwelveOperatorSynthEditor(TwelveOperatorSynthProcessor&);
    ~TwelveOperatorSynthEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    TwelveOperatorSynthProcessor& processor;
    juce::AudioProcessorValueTreeState& apvts;

    //==============================================================================
    // Header
    juce::Label titleLabel;

    //==============================================================================
    // Tabs
    juce::TabbedComponent tabs { juce::TabbedButtonBar::TabsAtTop };

    //==============================================================================
    // Synthesis tab
    juce::ComboBox  modeCombo, algoCombo;
    juce::Slider    gainSlider, driftSlider;
    juce::Label     modeLabel, algoLabel, gainLabel, driftLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeAttach, algoAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   gainAttach, driftAttach;

    //==============================================================================
    // Envelope tab
    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider;
    juce::Label  attackLabel, decayLabel, sustainLabel, releaseLabel;
    juce::ToggleButton vactrolToggle;
    juce::Slider vactrolAttackSlider, vactrolReleaseSlider, vactrolSagSlider, vactrolCurveSlider;
    juce::Label  vactrolAttackLabel, vactrolReleaseLabel, vactrolSagLabel, vactrolCurveLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        attackAttach, decayAttach, sustainAttach, releaseAttach,
        vactrolAttackAttach, vactrolReleaseAttach, vactrolSagAttach, vactrolCurveAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> vactrolEnableAttach;

    //==============================================================================
    // Filter tab
    juce::Slider hpCutSlider, hpQSlider, lpCutSlider, lpQSlider;
    juce::Label  hpCutLabel, hpQLabel, lpCutLabel, lpQLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        hpCutAttach, hpQAttach, lpCutAttach, lpQAttach;

    //==============================================================================
    // FX tab
    juce::ToggleButton chorusToggle, delayToggle, reverbToggle, compToggle;
    juce::Slider  chorusRateSlider, chorusDepthSlider, chorusMixSlider;
    juce::Slider  delayTimeSlider,  delayFeedSlider,   delayMixSlider;
    juce::Slider  reverbSizeSlider, reverbDampSlider,  reverbMixSlider;
    juce::Slider  compThreshSlider, compRatioSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        chorusEnableAttach, delayEnableAttach, reverbEnableAttach, compEnableAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        chorusRateAttach, chorusDepthAttach, chorusMixAttach,
        delayTimeAttach, delayFeedAttach, delayMixAttach,
        reverbSizeAttach, reverbDampAttach, reverbMixAttach,
        compThreshAttach, compRatioAttach;

    //==============================================================================
    // Tab component holders
    juce::Component synthTab, envTab, filterTab, fxTab;

    void buildSynthTab();
    void buildEnvTab();
    void buildFilterTab();
    void buildFxTab();

    static void styleSlider(juce::Slider& s, juce::Label& l, const juce::String& name);
    static void styleRotary(juce::Slider& s);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TwelveOperatorSynthEditor)
};
