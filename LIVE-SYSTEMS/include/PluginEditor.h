#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

//==============================================================================
/**
    Base class for LIVE-SYSTEMS plugin editors.
    Provides a modern, customizable GUI framework with theme support and responsive design.
*/
class LiveSystemsEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    LiveSystemsEditor(LiveSystemsProcessor&);
    ~LiveSystemsEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

protected:
    // Framework methods for derived classes to override
    virtual void setupComponents() = 0;
    virtual void paintBackground(juce::Graphics& g) = 0;
    virtual void layoutComponents() = 0;

    // Theme and styling
    struct Theme
    {
        juce::Colour backgroundColour{ 0xff2d2d2d };
        juce::Colour primaryColour{ 0xff0080ff };
        juce::Colour secondaryColour{ 0xff404040 };
        juce::Colour textColour{ 0xffffffff };
        juce::Colour outlineColour{ 0xff606060 };
        
        juce::Font primaryFont{ 14.0f };
        juce::Font headerFont{ 18.0f, juce::Font::bold };
    };

    Theme currentTheme;
    LiveSystemsProcessor& audioProcessor;

    /// Call this in derived constructors to mark components as initialized.
    /// If not called, setupComponents() will be auto-called on first resized().
    void markComponentsReady() { componentsInitialized = true; }

private:
    bool componentsInitialized = false;
    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LiveSystemsEditor)
};