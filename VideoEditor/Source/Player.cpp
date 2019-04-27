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
}

Player::~Player()
{
    shutDown();
}

void Player::start()
{
    transportSource.start();
}

void Player::stop()
{
    transportSource.stop();
}

bool Player::isPlaying()
{
    return transportSource.isPlaying();
}

void Player::setPosition (double pts)
{
    if (clip)
        clip->setNextReadPosition (pts * getSampleRate());
}

foleys::Timecode Player::getCurrentTimecode() const
{
    if (clip)
        return clip->getCurrentTimecode();

    return {};
}

double Player::getCurrentTimeInSeconds() const
{
    if (clip)
        return clip->getCurrentTimeInSeconds();

    return {};
}

void Player::setClip (std::shared_ptr<foleys::AVClip> clipToUse)
{
    transportSource.stop();
    transportSource.setSource (nullptr);
    clip = clipToUse;
    if (auto* device = deviceManager.getCurrentAudioDevice())
        if (clip != nullptr)
            clip->prepareToPlay (device->getDefaultBufferSize(), device->getCurrentSampleRate());

    transportSource.setSource (clip.get());

    preview.setClip (clip);
}

void Player::initialise ()
{
    deviceManager.initialise (0, 2, nullptr, true);
    deviceManager.addChangeListener (this);

    sourcePlayer.setSource (&transportSource);
    deviceManager.addAudioCallback (&sourcePlayer);
}

void Player::shutDown ()
{
    deviceManager.removeChangeListener (this);
    sourcePlayer.setSource (nullptr);
    deviceManager.removeAudioCallback (&sourcePlayer);
}

double Player::getSampleRate() const
{
    if (deviceManager.getCurrentAudioDevice() != nullptr)
        return deviceManager.getCurrentAudioDevice()->getCurrentSampleRate();

    return 0;
}

void Player::changeListenerCallback (ChangeBroadcaster* sender)
{
    if (auto* device = deviceManager.getCurrentAudioDevice())
        if (clip != nullptr)
            clip->prepareToPlay (device->getDefaultBufferSize(), device->getCurrentSampleRate());
}

FFAU::LevelMeterSource& Player::getMeterSource()
{
    return transportSource.meterSource;
}
