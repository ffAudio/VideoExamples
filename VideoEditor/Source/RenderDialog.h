/*
  ==============================================================================

    RenderDialog.h
    Created: 23 Apr 2019 8:36:47pm
    Author:  Daniel Walz

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class RenderDialog    : public Component,
                        private Timer
{
public:
    RenderDialog (foleys::ClipBouncer& renderer);
    ~RenderDialog();

//    void paint (Graphics&) override;
    void resized() override;

    void timerCallback() override;

private:

    void updateGUI();

    foleys::ClipBouncer& renderer;

    Label       filename;
    TextButton  browse { NEEDS_TRANS ("Browse") };
    double      progress = 0.0;
    ProgressBar progressBar { progress };
    TextButton  cancel { NEEDS_TRANS ("Cancel") };
    TextButton  start  { NEEDS_TRANS ("Start") };


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RenderDialog)
};
