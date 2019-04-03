/*
  ==============================================================================

    Main GUI for the components for the VideoEditor

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "Player.h"
#include "Library.h"
#include "Properties.h"
#include "TimeLine.h"
#include "TransportControl.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public Component,
                        public DragAndDropContainer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    //==============================================================================

    AudioDeviceManager    deviceManager;

    FFAU::LevelMeterLookAndFeel lookAndFeel;

    Library               library;
    foleys::VideoPreview  preview;
    Properties            properties { deviceManager };
    TimeLine              timeline   { player };
    TransportControl      transport  { player };
    FFAU::LevelMeter      levelMeter;

    Player                player { deviceManager, preview };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
