/*
  ==============================================================================

    Main GUI for the components for the VideoEditor

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    addAndMakeVisible (library);
    addAndMakeVisible (preview);
    addAndMakeVisible (properties);
    addAndMakeVisible (timeline);
    addAndMakeVisible (transport);

    const auto area = Desktop::getInstance().getDisplays().getMainDisplay().userArea;
    setBounds (area);

    player.initialise();
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    timeline.setBounds (bounds.removeFromBottom (bounds.getHeight() * 0.4));
    auto sides = bounds.getWidth() / 4.0;
    library.setBounds (bounds.removeFromLeft (sides));
    properties.setBounds (bounds.removeFromRight (sides));
    transport.setBounds (bounds.removeFromBottom (24));
    preview.setBounds (bounds);
}
