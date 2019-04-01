/*
  ==============================================================================

    TimeLine.h
    Created: 30 Mar 2019 4:46:00pm
    Author:  Daniel Walz

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class TimeLine    : public Component,
                    public DragAndDropTarget,
                    public FileDragAndDropTarget
{
public:
    TimeLine (Player& player);
    ~TimeLine();

    bool isInterestedInFileDrag (const StringArray& files) override;
    void filesDropped (const StringArray& files, int x, int y) override;
    bool isInterestedInDragSource (const SourceDetails &dragSourceDetails) override;
    void itemDropped (const SourceDetails &dragSourceDetails) override;

    void paint (Graphics&) override;

private:
    Player& player;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeLine)
};
