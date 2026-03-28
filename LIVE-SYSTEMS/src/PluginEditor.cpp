#include "../include/PluginEditor.h"

//==============================================================================
LiveSystemsEditor::LiveSystemsEditor(LiveSystemsProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set default size - can be overridden by derived classes
    setSize(800, 600);
    
    // Note: Don't call setupComponents() here as it's pure virtual
    // Derived classes must call it in their constructor after this constructor completes
    
    // Start timer for GUI updates
    startTimerHz(30); // 30 FPS refresh rate
}

LiveSystemsEditor::~LiveSystemsEditor()
{
    stopTimer();
}

//==============================================================================
void LiveSystemsEditor::paint(juce::Graphics& g)
{
    // Paint the background using the theme
    paintBackground(g);
}

void LiveSystemsEditor::resized()
{
    // Auto-initialize components if derived class didn't call setupComponents()
    if (!componentsInitialized)
    {
        setupComponents();
        componentsInitialized = true;
    }
    // Layout components (implemented by derived class)
    layoutComponents();
}

void LiveSystemsEditor::timerCallback()
{
    // Update GUI elements that need real-time updates
    // This can be overridden by derived classes for custom updates
    repaint();
}