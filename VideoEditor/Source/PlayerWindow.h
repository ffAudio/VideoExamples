/*
  ==============================================================================

    PlayerWindow.h
    Created: 8 Oct 2020 9:42:22pm
    Author:  Daniel Walz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class PlayerWindow  : public juce::TopLevelWindow
{
public:
    PlayerWindow();

    void resized() override;

    foleys::VideoPreview video;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlayerWindow)
};