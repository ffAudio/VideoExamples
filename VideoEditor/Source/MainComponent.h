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
                        public DragAndDropContainer,
                        private ApplicationCommandTarget,
                        public MenuBarModel,
                        private Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    void timerCallback() override;

    ApplicationCommandTarget* getNextCommandTarget() override { return nullptr; }
    void getAllCommands (Array<CommandID>& commands) override;
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform (const InvocationInfo& info) override;

    StringArray getMenuBarNames() override;
    PopupMenu getMenuForIndex (int topLevelMenuIndex,
                               const String& menuName) override;
    void menuItemSelected (int menuItemID,
                           int topLevelMenuIndex) override {}

private:

    void resetEdit();
    void loadEdit();
    void saveEdit (bool saveAs);

    void deleteSelectedClip();

    //==============================================================================

    AudioDeviceManager    deviceManager;
    foleys::VideoEngine   videoEngine;

    FFAU::LevelMeterLookAndFeel lookAndFeel;
    ApplicationCommandManager   commandManager;

    Library               library;
    foleys::VideoPreview  preview;
    Properties            properties { deviceManager };
    Viewport              viewport;
    TimeLine              timeline   { videoEngine, player };
    TransportControl      transport  { player };
    FFAU::LevelMeter      levelMeter { FFAU::LevelMeter::Default };

    Player                player { deviceManager, preview };

    File editFileName;
    int  lowerPart = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
