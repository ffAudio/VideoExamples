/*
  ==============================================================================

    Main GUI for the components for the VideoEditor

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setLookAndFeel (&lookAndFeel);

    addAndMakeVisible (library);
    addAndMakeVisible (preview);
    addAndMakeVisible (properties);
    addAndMakeVisible (timeline);
    addAndMakeVisible (transport);
    addAndMakeVisible (levelMeter);

    const auto area = Desktop::getInstance().getDisplays().getMainDisplay().userArea;
    setBounds (area);

    player.initialise();
    levelMeter.setMeterSource (&player.getMeterSource());
}

MainComponent::~MainComponent()
{
    setLookAndFeel (nullptr);
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    auto lower  = bounds.removeFromBottom (bounds.getHeight() * 0.4);
    levelMeter.setBounds (lower.removeFromRight (lower.getHeight() / 4));
    timeline.setBounds (lower);
    auto sides = bounds.getWidth() / 4.0;
    library.setBounds (bounds.removeFromLeft (sides));
    properties.setBounds (bounds.removeFromRight (sides));
    transport.setBounds (bounds.removeFromBottom (24));
    preview.setBounds (bounds);
}
