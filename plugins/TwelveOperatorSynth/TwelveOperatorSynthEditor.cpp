#include "TwelveOperatorSynthEditor.h"

namespace
{
constexpr int editorW = 620;
constexpr int editorH = 460;
constexpr int headerH = 44;

const juce::Colour bgColour      { 0xff1a1a2e };
const juce::Colour panelColour   { 0xff16213e };
const juce::Colour accentColour  { 0xff0f3460 };
const juce::Colour highlightCol  { 0xffe94560 };
const juce::Colour textColour    { 0xffeaeaea };
}

// ============================================================================
TwelveOperatorSynthEditor::TwelveOperatorSynthEditor(TwelveOperatorSynthProcessor& p)
    : AudioProcessorEditor(p), processor(p), apvts(p.getAPVTS())
{
    setSize(editorW, editorH);

    // Title
    titleLabel.setText("12-Op FM Synthesizer", juce::dontSendNotification);
    titleLabel.setFont(juce::Font("Arial", 22.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, highlightCol);
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(titleLabel);

    // Build tab contents
    buildSynthTab();
    buildEnvTab();
    buildFilterTab();
    buildFxTab();

    // Tabs
    tabs.addTab("Synthesis",  panelColour, &synthTab,  false);
    tabs.addTab("Envelopes",  panelColour, &envTab,    false);
    tabs.addTab("Filters",    panelColour, &filterTab, false);
    tabs.addTab("FX Chain",   panelColour, &fxTab,     false);
    tabs.setColour(juce::TabbedComponent::backgroundColourId, bgColour);
    tabs.setColour(juce::TabbedButtonBar::tabTextColourId, textColour);
    addAndMakeVisible(tabs);
}

TwelveOperatorSynthEditor::~TwelveOperatorSynthEditor() = default;

// ============================================================================
void TwelveOperatorSynthEditor::paint(juce::Graphics& g)
{
    g.fillAll(bgColour);

    // Header bar
    g.setColour(accentColour);
    g.fillRect(0, 0, getWidth(), headerH);

    g.setColour(highlightCol);
    g.drawLine(0.0f, static_cast<float>(headerH), static_cast<float>(getWidth()),
               static_cast<float>(headerH), 2.0f);
}

void TwelveOperatorSynthEditor::resized()
{
    titleLabel.setBounds(12, 0, 400, headerH);
    tabs.setBounds(0, headerH, getWidth(), getHeight() - headerH);
}

// ============================================================================
// Helpers
// ============================================================================
void TwelveOperatorSynthEditor::styleRotary(juce::Slider& s)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 16);
    s.setColour(juce::Slider::rotarySliderFillColourId,   highlightCol);
    s.setColour(juce::Slider::rotarySliderOutlineColourId, accentColour);
    s.setColour(juce::Slider::textBoxTextColourId,         textColour);
    s.setColour(juce::Slider::textBoxOutlineColourId,      juce::Colours::transparentBlack);
}

void TwelveOperatorSynthEditor::styleSlider(juce::Slider& s, juce::Label& l,
                                            const juce::String& name)
{
    styleRotary(s);
    l.setText(name, juce::dontSendNotification);
    l.setFont(juce::Font(11.0f));
    l.setColour(juce::Label::textColourId, textColour);
    l.setJustificationType(juce::Justification::centred);
}

// ============================================================================
// Synthesis tab
// ============================================================================
void TwelveOperatorSynthEditor::buildSynthTab()
{
    synthTab.setSize(editorW, editorH - headerH - 30);

    // Combo labels
    for (auto* lbl : { &modeLabel, &algoLabel })
    {
        lbl->setFont(juce::Font(11.0f));
        lbl->setColour(juce::Label::textColourId, textColour);
        lbl->setJustificationType(juce::Justification::centredLeft);
        synthTab.addAndMakeVisible(lbl);
    }
    modeLabel.setText("Mode", juce::dontSendNotification);
    algoLabel.setText("Algorithm", juce::dontSendNotification);

    for (auto* cb : { &modeCombo, &algoCombo })
    {
        cb->setColour(juce::ComboBox::backgroundColourId, accentColour);
        cb->setColour(juce::ComboBox::textColourId, textColour);
        cb->setColour(juce::ComboBox::arrowColourId, highlightCol);
        synthTab.addAndMakeVisible(cb);
    }

    modeAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_MODE, modeCombo);
    algoAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_ALGORITHM, algoCombo);

    styleSlider(gainSlider, gainLabel, "Master Gain");
    styleSlider(driftSlider, driftLabel, "Analog Drift");
    for (auto* w : { static_cast<juce::Component*>(&gainSlider), &gainLabel,
                     &driftSlider, &driftLabel })
        synthTab.addAndMakeVisible(w);

    gainAttach  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_MASTER_GAIN, gainSlider);
    driftAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_ANALOG_DRIFT, driftSlider);

    // Layout – using explicit lambdas for clarity
    synthTab.setSize(editorW, editorH - headerH - 32);

    const int pad = 16;
    const int comboH = 24;
    const int labelH = 16;
    const int knobSize = 72;

    modeLabel.setBounds(pad, pad, 100, labelH);
    modeCombo.setBounds(pad, pad + labelH + 2, 180, comboH);
    algoLabel.setBounds(pad, pad + labelH + 2 + comboH + 8, 100, labelH);
    algoCombo.setBounds(pad, pad + labelH * 2 + 2 + comboH + 8 + 2, 220, comboH);

    const int knobY = pad;
    gainLabel.setBounds(editorW - 2 * (knobSize + pad), knobY, knobSize, labelH);
    gainSlider.setBounds(editorW - 2 * (knobSize + pad), knobY + labelH, knobSize, knobSize + 16);

    driftLabel.setBounds(editorW - (knobSize + pad), knobY, knobSize, labelH);
    driftSlider.setBounds(editorW - (knobSize + pad), knobY + labelH, knobSize, knobSize + 16);
}

// ============================================================================
// Envelope tab
// ============================================================================
void TwelveOperatorSynthEditor::buildEnvTab()
{
    envTab.setSize(editorW, editorH - headerH - 30);

    struct KnobDef { juce::Slider& s; juce::Label& l; const char* name; const char* paramID; };
    const KnobDef adsr[] = {
        { attackSlider,  attackLabel,  "Attack",  TwelveOperatorSynthProcessor::PARAM_ATTACK.toRawUTF8() },
        { decaySlider,   decayLabel,   "Decay",   TwelveOperatorSynthProcessor::PARAM_DECAY.toRawUTF8()  },
        { sustainSlider, sustainLabel, "Sustain", TwelveOperatorSynthProcessor::PARAM_SUSTAIN.toRawUTF8() },
        { releaseSlider, releaseLabel, "Release", TwelveOperatorSynthProcessor::PARAM_RELEASE.toRawUTF8() }
    };

    const int knobSize = 70;
    const int pad = 16;
    const int labelH = 16;
    int x = pad;

    for (const auto& k : adsr)
    {
        styleSlider(k.s, k.l, k.name);
        k.l.setBounds(x, pad, knobSize, labelH);
        k.s.setBounds(x, pad + labelH, knobSize, knobSize + 16);
        envTab.addAndMakeVisible(k.s);
        envTab.addAndMakeVisible(k.l);
        x += knobSize + 8;
    }

    attackAttach  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_ATTACK, attackSlider);
    decayAttach   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_DECAY, decaySlider);
    sustainAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_SUSTAIN, sustainSlider);
    releaseAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_RELEASE, releaseSlider);

    // Vactrol section
    const int vactrolY = pad + labelH + knobSize + 24;

    vactrolToggle.setButtonText("Vactrol Envelope");
    vactrolToggle.setColour(juce::ToggleButton::textColourId, highlightCol);
    vactrolToggle.setColour(juce::ToggleButton::tickColourId, highlightCol);
    vactrolToggle.setBounds(pad, vactrolY, 160, 22);
    envTab.addAndMakeVisible(vactrolToggle);
    vactrolEnableAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_VACTROL_ENABLE, vactrolToggle);

    struct VKnobDef { juce::Slider& s; juce::Label& l; const char* name; const char* id; };
    const VKnobDef vknobs[] = {
        { vactrolAttackSlider,  vactrolAttackLabel,  "V.Attack",  TwelveOperatorSynthProcessor::PARAM_VACTROL_ATTACK.toRawUTF8()  },
        { vactrolReleaseSlider, vactrolReleaseLabel, "V.Release", TwelveOperatorSynthProcessor::PARAM_VACTROL_RELEASE.toRawUTF8() },
        { vactrolSagSlider,     vactrolSagLabel,     "Sag",       TwelveOperatorSynthProcessor::PARAM_VACTROL_SAG.toRawUTF8()    },
        { vactrolCurveSlider,   vactrolCurveLabel,   "Curve",     TwelveOperatorSynthProcessor::PARAM_VACTROL_CURVE.toRawUTF8()  }
    };

    x = pad;
    for (const auto& k : vknobs)
    {
        styleSlider(k.s, k.l, k.name);
        k.l.setBounds(x, vactrolY + 26, knobSize, labelH);
        k.s.setBounds(x, vactrolY + 26 + labelH, knobSize, knobSize + 16);
        envTab.addAndMakeVisible(k.s);
        envTab.addAndMakeVisible(k.l);
        x += knobSize + 8;
    }

    vactrolAttackAttach  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_VACTROL_ATTACK, vactrolAttackSlider);
    vactrolReleaseAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_VACTROL_RELEASE, vactrolReleaseSlider);
    vactrolSagAttach     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_VACTROL_SAG, vactrolSagSlider);
    vactrolCurveAttach   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_VACTROL_CURVE, vactrolCurveSlider);
}

// ============================================================================
// Filter tab
// ============================================================================
void TwelveOperatorSynthEditor::buildFilterTab()
{
    const int knobSize = 80;
    const int pad = 20;
    const int labelH = 16;

    struct FKnob { juce::Slider& s; juce::Label& l; const char* name; const char* id; };
    const FKnob fknobs[] = {
        { hpCutSlider, hpCutLabel, "HP Cutoff",    TwelveOperatorSynthProcessor::PARAM_HP_CUTOFF.toRawUTF8() },
        { hpQSlider,   hpQLabel,   "HP Resonance", TwelveOperatorSynthProcessor::PARAM_HP_Q.toRawUTF8()      },
        { lpCutSlider, lpCutLabel, "LP Cutoff",    TwelveOperatorSynthProcessor::PARAM_LP_CUTOFF.toRawUTF8() },
        { lpQSlider,   lpQLabel,   "LP Resonance", TwelveOperatorSynthProcessor::PARAM_LP_Q.toRawUTF8()      }
    };

    int x = pad;
    for (const auto& k : fknobs)
    {
        styleSlider(k.s, k.l, k.name);
        k.l.setBounds(x, pad, knobSize, labelH);
        k.s.setBounds(x, pad + labelH, knobSize, knobSize + 20);
        filterTab.addAndMakeVisible(k.s);
        filterTab.addAndMakeVisible(k.l);
        x += knobSize + 20;
    }

    hpCutAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_HP_CUTOFF, hpCutSlider);
    hpQAttach   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_HP_Q, hpQSlider);
    lpCutAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_LP_CUTOFF, lpCutSlider);
    lpQAttach   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_LP_Q, lpQSlider);
}

// ============================================================================
// FX tab
// ============================================================================
void TwelveOperatorSynthEditor::buildFxTab()
{
    const int sectionW = (editorW - 8) / 4;
    const int pad = 6;
    const int knobSize = 62;

    // Helper: style toggle button and add it to fxTab
    auto addToggle = [&](juce::ToggleButton& btn, const char* text, int x)
    {
        btn.setButtonText(text);
        btn.setColour(juce::ToggleButton::textColourId, highlightCol);
        btn.setColour(juce::ToggleButton::tickColourId, highlightCol);
        btn.setBounds(x, pad, sectionW - pad, 22);
        fxTab.addAndMakeVisible(btn);
    };

    // Helper: style rotary knob and add it to fxTab
    auto addKnob = [&](juce::Slider& s, int x, int& y)
    {
        styleRotary(s);
        s.setBounds(x, y, knobSize, knobSize + 20);
        fxTab.addAndMakeVisible(s);
        y += knobSize + 22;
    };

    // ---- Chorus ----
    int sx = pad;
    addToggle(chorusToggle, "Chorus", sx);
    chorusEnableAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_CHORUS_ENABLE, chorusToggle);

    int ky = pad + 28;
    addKnob(chorusRateSlider,  sx, ky);
    addKnob(chorusDepthSlider, sx, ky);
    addKnob(chorusMixSlider,   sx, ky);
    chorusRateAttach  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_CHORUS_RATE, chorusRateSlider);
    chorusDepthAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_CHORUS_DEPTH, chorusDepthSlider);
    chorusMixAttach   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_CHORUS_MIX, chorusMixSlider);

    // ---- Delay ----
    sx += sectionW;
    addToggle(delayToggle, "Delay", sx);
    delayEnableAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_DELAY_ENABLE, delayToggle);

    ky = pad + 28;
    addKnob(delayTimeSlider, sx, ky);
    addKnob(delayFeedSlider, sx, ky);
    addKnob(delayMixSlider,  sx, ky);
    delayTimeAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_DELAY_TIME, delayTimeSlider);
    delayFeedAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_DELAY_FEEDBACK, delayFeedSlider);
    delayMixAttach  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_DELAY_MIX, delayMixSlider);

    // ---- Reverb ----
    sx += sectionW;
    addToggle(reverbToggle, "Reverb", sx);
    reverbEnableAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_REVERB_ENABLE, reverbToggle);

    ky = pad + 28;
    addKnob(reverbSizeSlider, sx, ky);
    addKnob(reverbDampSlider, sx, ky);
    addKnob(reverbMixSlider,  sx, ky);
    reverbSizeAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_REVERB_SIZE, reverbSizeSlider);
    reverbDampAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_REVERB_DAMP, reverbDampSlider);
    reverbMixAttach  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_REVERB_MIX, reverbMixSlider);

    // ---- Compressor ----
    sx += sectionW;
    addToggle(compToggle, "Compressor", sx);
    compEnableAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_COMP_ENABLE, compToggle);

    ky = pad + 28;
    addKnob(compThreshSlider, sx, ky);
    addKnob(compRatioSlider,  sx, ky);
    compThreshAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_COMP_THRESHOLD, compThreshSlider);
    compRatioAttach  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, TwelveOperatorSynthProcessor::PARAM_COMP_RATIO, compRatioSlider);
}
