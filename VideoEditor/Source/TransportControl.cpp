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

    addAndMakeVisible (zero);
    zero.onClick = [&]
    {
        player.setPosition (0);
    };

    zero.setConnectedEdges (TextButton::ConnectedOnRight);
    play.setConnectedEdges (TextButton::ConnectedOnLeft);

    player.addChangeListener (this);
}

TransportControl::~TransportControl()
{
    player.removeChangeListener (this);
}

void TransportControl::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.setColour (Colours::silver);

    auto bounds = getLocalBounds().reduced (1);
    g.drawFittedText (foleys::timecodeToString (player.getCurrentTimecode()), bounds, Justification::right, 1);
}

void TransportControl::resized()
{
    auto bounds = getLocalBounds().reduced (1);
    zero.setBounds (bounds.removeFromLeft (80));
    play.setBounds (bounds.removeFromLeft (80));
}

void TransportControl::timerCallback()
{
    repaint();
}

void TransportControl::changeListenerCallback (ChangeBroadcaster*)
{
    if (player.isPlaying())
    {
        play.setButtonText (NEEDS_TRANS ("Pause"));
        startTimerHz (30);
    }
    else
    {
        play.setButtonText (NEEDS_TRANS ("Play"));
        stopTimer();
    }

    juce::Timer::callAfterDelay (200, [&]{ repaint(); });
}
