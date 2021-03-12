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
#include "TimeMeter.h"
#include "TransportControl.h"
#include "PlayerWindow.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public juce::Component,
                        public juce::DragAndDropContainer,
                        private juce::ApplicationCommandTarget,
                        public juce::MenuBarModel,
                        private juce::Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    enum class Mode
    {
        NormalView = 0,
        MaximiseView,
        ExtraWindow
    };

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void setViewerFullScreen (Mode mode);

    void timerCallback() override;

    juce::ApplicationCommandTarget* getNextCommandTarget() override { return nullptr; }
    void getAllCommands (juce::Array<CommandID>& commands) override;
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform (const InvocationInfo& info) override;

    juce::StringArray getMenuBarNames() override;
    juce::PopupMenu getMenuForIndex (int topLevelMenuIndex,
                               const String& menuName) override;
    void menuItemSelected (int, int) override {}

    juce::KeyPressMappingSet* getKeyMappings() const;

    void loadEditFile (const juce::File& file);

    bool handleQuitRequest();

private:

    void setUseOpenGL (bool shouldUseOpenGL);

    void resetEdit();
    void loadEdit();
    void saveEdit (bool saveAs);
    void showRenderDialog();

    void deleteSelectedClip();
    void showPreferences();

    void updateTitleBar();

    //==============================================================================

    juce::AudioDeviceManager deviceManager;
    foleys::VideoEngine   videoEngine;
    foleys::ClipRenderer  renderer { videoEngine };

    juce::ApplicationCommandManager    commandManager;

    std::unique_ptr<foleys::VideoView> preview;

    Player                player     { deviceManager, videoEngine };

    Library               library    { player, videoEngine };
    Properties            properties;
    Viewport              viewport;
    TimeLine              timeline   { videoEngine, player, properties };
    TimeMeter             timeMeter  { timeline, viewport.getHorizontalScrollBar() };
    TransportControl      transport  { player };
    juce::Slider          zoom       { juce::Slider::LinearHorizontal, juce::Slider::NoTextBox };

    foleys::LevelMeterLookAndFeel   lmLookAndFeel;
    foleys::LevelMeter              levelMeter;

    std::unique_ptr<PlayerWindow>   playerWindow;

    juce::File editFileName;
    int  lowerPart = 0;
    Mode viewerFullscreenMode = Mode::NormalView;
    bool usesOpenGL = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
