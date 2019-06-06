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
class Player  : public ChangeBroadcaster,
                public ChangeListener
{
public:
    Player (AudioDeviceManager& deviceManager, foleys::VideoPreview& preview);
    ~Player();

    void setClip (std::shared_ptr<foleys::AVClip> clip);

    void start();
    void stop();
    bool isPlaying();

    void setPosition (double pts);

    double getCurrentTimeInSeconds() const;

    FFAU::LevelMeterSource& getMeterSource();

    void initialise();
    void shutDown();

    double getSampleRate() const;

    void changeListenerCallback (ChangeBroadcaster* sender) override;

    class MeasuredTransportSource : public AudioTransportSource
    {
    public:
        MeasuredTransportSource() = default;

        void getNextAudioBlock (const AudioSourceChannelInfo& info) override
        {
            AudioTransportSource::getNextAudioBlock (info);
            AudioBuffer<float> proxy (info.buffer->getArrayOfWritePointers(),
                                      info.buffer->getNumChannels(),
                                      info.startSample,
                                      info.numSamples);
            meterSource.measureBlock (proxy);

            if (clipOutput)
            {
                for (int channel = 0; channel < info.buffer->getNumChannels(); ++channel)
                    FloatVectorOperations::clip (info.buffer->getWritePointer (channel, info.startSample),
                                                 info.buffer->getReadPointer (channel, info.startSample),
                                                 -1.0f, 1.0f, info.numSamples);
            }
        }

        FFAU::LevelMeterSource meterSource;

    private:
        bool clipOutput = true;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MeasuredTransportSource)
    };
private:
    AudioDeviceManager& deviceManager;

    std::shared_ptr<foleys::AVClip> clip;
    MeasuredTransportSource     transportSource;
    AudioSourcePlayer           sourcePlayer;
    foleys::VideoPreview&       preview;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Player)
};
