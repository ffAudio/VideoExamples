/*
  ==============================================================================

    Copyright (c) 2019, Foleys Finest Audio - Daniel Walz
    All rights reserved.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
    OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
    OF THE POSSIBILITY OF SUCH DAMAGE.

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
    sendChangeMessage();
}

void Player::stop()
{
    transportSource.stop();
    sendChangeMessage();
}

bool Player::isPlaying()
{
    return transportSource.isPlaying();
}

void Player::setPosition (double pts)
{
    if (clip)
        clip->setNextReadPosition (pts * getSampleRate());

    sendChangeMessage();
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

    sendChangeMessage();
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
