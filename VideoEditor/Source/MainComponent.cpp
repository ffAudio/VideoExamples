/*
  ==============================================================================

    Main GUI for the components for the VideoEditor

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    levelMeter.setLookAndFeel (&lookAndFeel);
    lookAndFeel.setColour (FFAU::LevelMeter::lmBackgroundColour, getLookAndFeel().findColour (ResizableWindow::backgroundColourId).darker());
    lookAndFeel.setColour (FFAU::LevelMeter::lmTicksColour, Colours::silver);

    addAndMakeVisible (library);
    addAndMakeVisible (preview);
    addAndMakeVisible (properties);
    addAndMakeVisible (viewport);
    addAndMakeVisible (transport);
    addAndMakeVisible (levelMeter);

    viewport.setViewedComponent (&timeline);
    timeline.setSize (2000, 800);

    const auto area = Desktop::getInstance().getDisplays().getMainDisplay().userArea;
    setBounds (area);

    player.initialise();
    levelMeter.setMeterSource (&player.getMeterSource());

    auto edit = videoEngine.createCompoundClip();
    timeline.setEditClip (edit);
    edit->addTimecodeListener (&preview);
}

MainComponent::~MainComponent()
{
    if (auto edit = timeline.getEditClip())
        edit->removeTimecodeListener (&preview);

    levelMeter.setLookAndFeel (nullptr);
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.setColour (getLookAndFeel().findColour (ResizableWindow::backgroundColourId).darker());
    g.fillRect (getLocalBounds().withTop (lowerPart));
    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds());
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds().reduced (1);
    lowerPart = bounds.getHeight() * 0.4;
    auto lower  = bounds.removeFromBottom (lowerPart);
    levelMeter.setBounds (lower.removeFromRight (lower.getHeight() / 4).reduced (2));
    lower.removeFromTop (14); // TODO: ruler
    viewport.setBounds (lower);
    auto sides = bounds.getWidth() / 4.0;
    library.setBounds (bounds.removeFromLeft (sides));
    properties.setBounds (bounds.removeFromRight (sides));
    transport.setBounds (bounds.removeFromBottom (24));
    preview.setBounds (bounds);
}
