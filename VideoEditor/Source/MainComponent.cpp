/*
  ==============================================================================

    Main GUI for the components for the VideoEditor

  ==============================================================================
*/

#include "MainComponent.h"

namespace CommandIDs
{
    enum
    {
        fileOpen = 100,
        fileSave,
        fileSaveAs,
        fileNew,
        fileRender,
        fileQuit,

        editUndo = 200,
        editRedo,
        editDelete,
        editCopy,
        editPaste,

        playStart = 300,
        playStop,
        playReturn,
        playRecord,

        helpAbout = 400,
        helpHelp
    };
}


//==============================================================================
MainComponent::MainComponent()
{
    levelMeter.setLookAndFeel (&lookAndFeel);
    lookAndFeel.setColour (FFAU::LevelMeter::lmBackgroundColour, getLookAndFeel().findColour (ResizableWindow::backgroundColourId).darker());
    lookAndFeel.setColour (FFAU::LevelMeter::lmTicksColour, Colours::silver);

    addAndMakeVisible (library);
    addAndMakeVisible (preview);
    addAndMakeVisible (properties);
    addAndMakeVisible (viewport);
    addAndMakeVisible (transport);
    addAndMakeVisible (levelMeter);

    viewport.setViewedComponent (&timeline);
    timeline.setSize (2000, 580);

    const auto area = Desktop::getInstance().getDisplays().getMainDisplay().userArea;
    setBounds (area);

    player.initialise();
    levelMeter.setMeterSource (&player.getMeterSource());

    resetEdit();

    commandManager.registerAllCommandsForTarget (this);
    commandManager.setFirstCommandTarget (this);
#if JUCE_MAC
    setMacMainMenu (this);
#else
    if (auto* window = dynamic_cast<DocumentWindow*>(getTopLevelComponent()))
        window->setMenuBar (this);
#endif

    startTimerHz (10);
}

MainComponent::~MainComponent()
{
    if (auto edit = timeline.getEditClip())
        edit->removeTimecodeListener (&preview);

#if JUCE_MAC
    setMacMainMenu (nullptr);
#else
    if (auto* window = dynamic_cast<DocumentWindow*>(getTopLevelComponent()))
        window->setMenuBar (nullptr);
#endif

    levelMeter.setLookAndFeel (nullptr);
}

//==============================================================================

void MainComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.setColour (getLookAndFeel().findColour (ResizableWindow::backgroundColourId).darker());
    g.fillRect (getLocalBounds().withTop (lowerPart));
    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds());
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds().reduced (1);
    lowerPart = bounds.getHeight() * 0.4;
    auto lower  = bounds.removeFromBottom (lowerPart);
    levelMeter.setBounds (lower.removeFromRight (lower.getHeight() / 4).reduced (2));
    lower.removeFromTop (14); // TODO: ruler
    viewport.setBounds (lower);
    auto sides = bounds.getWidth() / 4.0;
    library.setBounds (bounds.removeFromLeft (sides));
    properties.setBounds (bounds.removeFromRight (sides));
    transport.setBounds (bounds.removeFromBottom (24));
    preview.setBounds (bounds);
}

void MainComponent::resetEdit()
{
    auto edit = videoEngine.createCompoundClip();
    timeline.setEditClip (edit);
    edit->addTimecodeListener (&preview);
}

void MainComponent::loadEdit()
{
    FileChooser myChooser ("Please select the project you want to save...",
                           File::getSpecialLocation (File::userHomeDirectory),
                           "*.videdit");
    if (myChooser.browseForFileToOpen())
    {
        std::unique_ptr<XmlElement> xml;
        xml.reset (XmlDocument::parse (myChooser.getResult()));
        if (xml == nullptr)
        {
            AlertWindow::showMessageBox (AlertWindow::WarningIcon,
                                         NEEDS_TRANS ("Loading failed"),
                                         "Loading of the file \"" + myChooser.getResult().getFullPathName() + "\" failed.");
            return;
        }

        auto tree = ValueTree::fromXml (*xml);
        auto edit = videoEngine.createCompoundClip();

        for (auto clip : tree)
            edit->getStatusTree().appendChild (clip.createCopy(), nullptr);

        timeline.setEditClip (edit);
        edit->addTimecodeListener (&preview);
    }
}

void MainComponent::saveEdit (bool saveAs)
{
    auto edit = timeline.getEditClip();

    if (saveAs || editFileName.getFullPathName().isEmpty())
    {
        FileChooser myChooser ("Please select the project you want to save...",
                               File::getSpecialLocation (File::userHomeDirectory),
                               "*.videdit");
        if (myChooser.browseForFileToSave (true))
        {
            editFileName = myChooser.getResult();
        }
        else
        {
            return;
        }
    }

    if (edit && editFileName.getFullPathName().isNotEmpty())
    {
        FileOutputStream output (editFileName);
        if (output.openedOk())
        {
            output.setPosition (0);
            output.truncate();
            output.writeString (edit->getStatusTree().toXmlString());
        }
        else
        {
            AlertWindow::showMessageBox (AlertWindow::WarningIcon, NEEDS_TRANS("Saving failed"), "Saving of file \"" + editFileName.getFullPathName() + "\" failed.");
        }
    }
}

void MainComponent::deleteSelectedClip()
{
    if (auto selected = timeline.getSelectedClip())
        if (auto edit = timeline.getEditClip())
            edit->removeClip (selected);
}

//==============================================================================

void MainComponent::getAllCommands (Array<CommandID>& commands)
{
    commands.add (CommandIDs::fileNew, CommandIDs::fileOpen, CommandIDs::fileSave, CommandIDs::fileSaveAs, CommandIDs::fileRender, CommandIDs::fileQuit);
    commands.add (CommandIDs::editUndo, CommandIDs::editRedo, CommandIDs::editDelete, CommandIDs::editCopy, CommandIDs::editPaste);
    commands.add (CommandIDs::helpAbout, CommandIDs::helpHelp);
}

void MainComponent::getCommandInfo (CommandID commandID, ApplicationCommandInfo& result)
{
    auto categoryFile = "file";
    auto categoryEdit = "edit";
    auto categoryHelp = "help";

    switch (commandID)
    {
        case CommandIDs::fileNew:
            result.setInfo ("New Project...", "Clear all and start fresh", categoryFile, 0);
            result.defaultKeypresses.add (KeyPress ('n', ModifierKeys::commandModifier, 0));
            break;
        case CommandIDs::fileOpen:
            result.setInfo ("Open Project...", "Select a project to open", categoryFile, 0);
            result.defaultKeypresses.add (KeyPress ('o', ModifierKeys::commandModifier, 0));
            break;
        case CommandIDs::fileSave:
            result.setInfo ("Save Project", "Save the current project", categoryFile, 0);
            result.defaultKeypresses.add (KeyPress ('s', ModifierKeys::commandModifier, 0));
            break;
        case CommandIDs::fileSaveAs:
            result.setInfo ("Save Project...", "Save the current project as new file", categoryFile, 0);
            break;
        case CommandIDs::fileRender:
            result.setInfo ("Render Project...", "Export the piece into an audio file", categoryFile, 0);
            result.defaultKeypresses.add (KeyPress ('r', ModifierKeys::commandModifier, 0));
            break;
        case CommandIDs::fileQuit:
            result.setInfo ("Quit...", "Quit Application", categoryFile, 0);
            result.defaultKeypresses.add (KeyPress ('q', ModifierKeys::commandModifier, 0));
            break;
        case CommandIDs::editUndo:
            result.setInfo ("Undo", "Undo the last step", categoryEdit, 0);
            result.defaultKeypresses.add (KeyPress ('z', ModifierKeys::commandModifier, 0));
            break;
        case CommandIDs::editRedo:
            result.setInfo ("Redo", "Redo the last undo step", categoryEdit, 0);
            result.defaultKeypresses.add (KeyPress ('z', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0));
            break;
        case CommandIDs::editDelete:
            result.setInfo ("Delete", "Delete the selected gesture", categoryEdit, 0);
            result.defaultKeypresses.add (KeyPress (KeyPress::backspaceKey, ModifierKeys::commandModifier, 0));
            break;
        case CommandIDs::editCopy:
            result.setInfo ("Copy", "Copy the selected gesture", categoryEdit, 0);
            result.defaultKeypresses.add (KeyPress ('c', ModifierKeys::commandModifier, 0));
            break;
        case CommandIDs::editPaste:
            result.setInfo ("Paste", "Paste the gesture in the clipboard", categoryEdit, 0);
            result.defaultKeypresses.add (KeyPress ('v', ModifierKeys::commandModifier, 0));
            break;
        case CommandIDs::helpAbout:
            result.setInfo ("About", "Show information about the program", categoryHelp, 0);
            break;
        case CommandIDs::helpHelp:
            result.setInfo ("Help", "Show help how to use the program", categoryHelp, 0);
            break;
        default:
            JUCEApplication::getInstance()->getCommandInfo (commandID, result);
            break;
    }
}

bool MainComponent::perform (const InvocationInfo& info)
{
    switch (info.commandID) {
        case CommandIDs::fileNew: resetEdit(); break;
        case CommandIDs::fileOpen: loadEdit(); break;
        case CommandIDs::fileSave: saveEdit(false); break;
        case CommandIDs::fileSaveAs: saveEdit(true); break;

        case CommandIDs::editUndo:   videoEngine.getUndoManager()->undo(); break;
        case CommandIDs::editRedo:   videoEngine.getUndoManager()->redo(); break;
        case CommandIDs::editDelete: deleteSelectedClip(); break;

        default:
            break;
    }
    return true;
}

StringArray MainComponent::getMenuBarNames()
{
    return {NEEDS_TRANS ("File"), NEEDS_TRANS ("Edit"), NEEDS_TRANS ("Play"), NEEDS_TRANS ("Help")};
}

PopupMenu MainComponent::getMenuForIndex (int topLevelMenuIndex,
                                          const String& menuName)
{
    PopupMenu menu;
    if (topLevelMenuIndex == 0)
    {
        menu.addCommandItem (&commandManager, CommandIDs::fileNew);
        menu.addSeparator();
        menu.addCommandItem (&commandManager, CommandIDs::fileOpen);
        menu.addCommandItem (&commandManager, CommandIDs::fileSave);
        menu.addCommandItem (&commandManager, CommandIDs::fileSaveAs);
        menu.addCommandItem (&commandManager, CommandIDs::fileRender);
#if ! JUCE_MAC
        menu.addSeparator();
        menu.addCommandItem (&commandManager, CommandIDs::fileQuit);
#endif
    }
    else if (topLevelMenuIndex == 1)
    {
        menu.addCommandItem (&commandManager, CommandIDs::editUndo);
        menu.addCommandItem (&commandManager, CommandIDs::editRedo);
        menu.addSeparator();
        menu.addCommandItem (&commandManager, CommandIDs::editDelete);
        menu.addCommandItem (&commandManager, CommandIDs::editCopy);
        menu.addCommandItem (&commandManager, CommandIDs::editPaste);
    }
    else if (topLevelMenuIndex == 2)
    {
        menu.addCommandItem (&commandManager, CommandIDs::playStart);
        menu.addCommandItem (&commandManager, CommandIDs::playStop);
        menu.addCommandItem (&commandManager, CommandIDs::playRecord);
    }
    else if (topLevelMenuIndex == 3)
    {
        menu.addCommandItem (&commandManager, CommandIDs::helpAbout);
        menu.addCommandItem (&commandManager, CommandIDs::helpHelp);
    }
    return menu;

}

void MainComponent::timerCallback()
{
    for (auto& source : Desktop::getInstance().getMouseSources())
        if (source.isDragging())
            return;

    videoEngine.getUndoManager()->beginNewTransaction();
}
