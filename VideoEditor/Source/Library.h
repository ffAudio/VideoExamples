/*
  ==============================================================================

    Library.h
    Created: 30 Mar 2019 4:45:01pm
    Author:  Daniel Walz

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class Library    : public Component
{
public:
    Library();
    ~Library();

    void paint (Graphics&) override;
    void resized() override;

    class MediaList : public Component
    {
    public:
        MediaList (TimeSliceThread& readThread, const File& root, std::unique_ptr<FileFilter> filter);
        void resized() override;

    private:
        std::unique_ptr<FileFilter> filter;
        DirectoryContentsList       contents;
        FileTreeComponent           fileTree  { contents };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MediaList)
    };
private:
    TimeSliceThread directoryThread { "Directory read thread" };

    TabbedComponent tabs { TabbedButtonBar::TabsAtTop };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Library)
};
