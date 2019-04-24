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
class Player : public ChangeListener
{
public:
    Player (AudioDeviceManager& deviceManager, foleys::VideoPreview& preview);
    ~Player();

    void setClip (std::shared_ptr<foleys::AVClip> clip);

    void start();
    void stop();
    bool isPlaying();

    void setPosition (double pts);

    foleys::Timecode getCurrentTimecode() const;

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
        bool preFader = true;
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
