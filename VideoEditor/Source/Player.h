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

    void setClip (std::shared_ptr<foleys::AVClip> clip);

    void start();
    void stop();
    bool isPlaying();

    void setPosition (double pts);

    foleys::Timecode getCurrentTimecode() const;

    FFAU::LevelMeterSource& getMeterSource();

    void initialise();
    void shutDown();

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
        }

        FFAU::LevelMeterSource meterSource;

    private:
        bool preFader = true;

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
