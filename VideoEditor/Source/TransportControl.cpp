/*
  ==============================================================================

    TransportControl.cpp
    Created: 1 Apr 2019 11:54:16pm
    Author:  Daniel Walz

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Player.h"
#include "TransportControl.h"

//==============================================================================
TransportControl::TransportControl (Player& playerToUse)
  : player (playerToUse)
{
    addAndMakeVisible (play);
    play.setClickingTogglesState (true);
    play.onStateChange = [&]
    {
        if (play.getToggleState())
            player.start();
        else
            player.stop();
    };
}

TransportControl::~TransportControl()
{
}

void TransportControl::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

void TransportControl::resized()
{
    auto bounds = getLocalBounds().reduced (1);
    play.setBounds (bounds.removeFromLeft (80));
}
