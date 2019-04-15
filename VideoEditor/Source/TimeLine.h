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
                    public FileDragAndDropTarget,
                    public foleys::AVClip::TimecodeListener,
                    public ValueTree::Listener
{
public:
    TimeLine (foleys::VideoEngine& videoEngine, Player& player);
    ~TimeLine();

    bool isInterestedInFileDrag (const StringArray& files) override;
    void filesDropped (const StringArray& files, int x, int y) override;
    bool isInterestedInDragSource (const SourceDetails &dragSourceDetails) override;
    void itemDropped (const SourceDetails &dragSourceDetails) override;

    void mouseDown (const MouseEvent& event) override;

    void paint (Graphics&) override;
    void resized() override;
    void timecodeChanged (foleys::Timecode tc) override;

    void setEditClip (std::shared_ptr<foleys::ComposedClip> clip);
    std::shared_ptr<foleys::ComposedClip> getEditClip() const;

    void setSelectedClip (std::shared_ptr<foleys::ComposedClip::ClipDescriptor> clip);
    std::shared_ptr<foleys::ComposedClip::ClipDescriptor> getSelectedClip() const;

    void restoreClipComponents();

    class ClipComponent : public Component
    {
    public:
        ClipComponent (TimeLine& tl, std::shared_ptr<foleys::ComposedClip::ClipDescriptor> clip, ThreadPool& threadPool, bool video);
        void paint (Graphics& g) override;
        void resized() override;

        void mouseMove (const MouseEvent& event) override;
        void mouseDown (const MouseEvent& event) override;
        void mouseDrag (const MouseEvent& event) override;
        void mouseUp (const MouseEvent& event) override;

        bool isVideoClip() const;

        std::shared_ptr<foleys::ComposedClip::ClipDescriptor> clip;

    private:
        enum DragMode
        {
            notDragging, dragPosition, dragLength
        };

        TimeLine& timeline;
        std::unique_ptr<foleys::FilmStrip> filmstrip;

        DragMode dragmode = notDragging;
        Point<int> localDragStart;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ClipComponent)
    };

    int getXFromTime (double seconds) const;
    double getTimeFromX (int pixels) const;

    double getSampleRate() const;

    class TimeMarker : public Component
    {
    public:
        TimeMarker() = default;
        void paint (Graphics& g)
        {
            g.setColour (Colours::red);
            g.drawVerticalLine (1, 0, getHeight());
        }
    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeMarker)
    };

    void valueTreePropertyChanged (juce::ValueTree& treeWhosePropertyHasChanged,
                                   const juce::Identifier& property) override;

    void valueTreeChildAdded (juce::ValueTree& parentTree,
                              juce::ValueTree& childWhichHasBeenAdded) override;

    void valueTreeChildRemoved (juce::ValueTree& parentTree,
                                juce::ValueTree& childWhichHasBeenRemoved,
                                int indexFromWhichChildWasRemoved) override;

    void valueTreeChildOrderChanged (juce::ValueTree& parentTreeWhoseChildrenHaveMoved,
                                     int oldIndex, int newIndex) override {}

    void valueTreeParentChanged (juce::ValueTree& treeWhoseParentHasChanged) override {}

private:

    void addClipToEdit (juce::File file, double start, int y);

    foleys::VideoEngine& videoEngine;
    Player& player;
    TimeMarker timemarker;

    const int numVideoLines = 2;
    const int numAudioLines = 5;

    std::shared_ptr<foleys::ComposedClip> edit;

    std::vector<std::unique_ptr<ClipComponent>> clipComponents;

    std::weak_ptr<foleys::ComposedClip::ClipDescriptor> selectedClip;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeLine)
};
