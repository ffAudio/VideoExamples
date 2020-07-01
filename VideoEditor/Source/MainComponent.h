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
    ~MainComponent() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    void setViewerFullScreen (bool shouldBeFullScreen);

    void timerCallback() override;

    ApplicationCommandTarget* getNextCommandTarget() override { return nullptr; }
    void getAllCommands (Array<CommandID>& commands) override;
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform (const InvocationInfo& info) override;

    StringArray getMenuBarNames() override;
    PopupMenu getMenuForIndex (int topLevelMenuIndex,
                               const String& menuName) override;
    void menuItemSelected (int, int) override {}

    KeyPressMappingSet* getKeyMappings() const;

    void loadEditFile (const File& file);

    bool handleQuitRequest();

private:

    void resetEdit();
    void loadEdit();
    void saveEdit (bool saveAs);
    void showRenderDialog();

    void deleteSelectedClip();
    void showPreferences();

    void updateTitleBar();

    //==============================================================================

    AudioDeviceManager    deviceManager;
    foleys::VideoEngine   videoEngine;
    foleys::ClipRenderer  renderer { videoEngine };

    ApplicationCommandManager   commandManager;

    foleys::VideoPreview  preview;
    Player                player  { deviceManager, videoEngine, preview };

    Library               library    { player, videoEngine };
    Properties            properties;
    Viewport              viewport;
    TimeLine              timeline   { videoEngine, player, properties };
    TransportControl      transport  { player };

    foleys::LevelMeterLookAndFeel   lmLookAndFeel;
    foleys::LevelMeter              levelMeter;

    File editFileName;
    int  lowerPart = 0;
    bool viewerFullScreen = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
