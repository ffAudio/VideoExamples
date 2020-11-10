/*
  ==============================================================================

    PlayerWindow.cpp
    Created: 8 Oct 2020 9:42:22pm
    Author:  Daniel Walz

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlayerWindow.h"

//==============================================================================
PlayerWindow::PlayerWindow() : juce::TopLevelWindow ("Output", true)
{
    setUsingNativeTitleBar (false);
    addAndMakeVisible (video);
}


void PlayerWindow::resized()
{
    video.setBounds (getLocalBounds());
}
