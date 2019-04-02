/*
  ==============================================================================

    TransportControl.h
    Created: 1 Apr 2019 11:54:16pm
    Author:  Daniel Walz

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class TransportControl    : public Component
{
public:
    TransportControl (Player& player);
    ~TransportControl();

    void paint (Graphics&) override;
    void resized() override;

private:
    Player& player;

    TextButton play { NEEDS_TRANS ("Play") };
    TextButton settings { NEEDS_TRANS ("Settings") };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransportControl)
};
