/*
  ==============================================================================

    Properties.h
    Created: 30 Mar 2019 4:45:22pm
    Author:  Daniel Walz

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class Properties    : public Component
{
public:
    Properties();
    ~Properties();

    void paint (Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Properties)
};
