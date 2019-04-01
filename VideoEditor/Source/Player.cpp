/*
  ==============================================================================

    Player.cpp
    Created: 31 Mar 2019 10:23:27pm
    Author:  Daniel Walz

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Player.h"

//==============================================================================
Player::Player (AudioDeviceManager& deviceManagerToUse, foleys::VideoPreview& previewToUse)
  : deviceManager (deviceManagerToUse),
    preview (previewToUse)
{
    sourcePlayer.setSource (&transportSource);
    deviceManagerToUse.addAudioCallback (&sourcePlayer);
}

Player::~Player()
{
    deviceManager.removeAudioCallback (&sourcePlayer);
}

void Player::setClip (std::unique_ptr<foleys::AVClip> clipToUse)
{
    transportSource.stop();
    transportSource.setSource (nullptr);
    clip = std::move (clipToUse);
    transportSource.setSource (clip.get());

    preview.setClip (clip.get());
}
