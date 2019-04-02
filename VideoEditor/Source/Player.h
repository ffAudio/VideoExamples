/*
  ==============================================================================

    Player.h
    Created: 31 Mar 2019 10:23:27pm
    Author:  Daniel Walz

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class Player
{
public:
    Player (AudioDeviceManager& deviceManager, foleys::VideoPreview& preview);
    ~Player();

    void setClip (std::unique_ptr<foleys::AVClip> clip);

    void start();
    void stop();
    bool isPlaying();

    void initialise();
    void shutDown();

private:
    AudioDeviceManager& deviceManager;

    std::unique_ptr<foleys::AVClip> clip;
    AudioTransportSource  transportSource;
    AudioSourcePlayer     sourcePlayer;
    foleys::VideoPreview& preview;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Player)
};
