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

    ProcessorComponent.cpp
    Created: 25 May 2019 11:18:43am
    Author:  Daniel Walz

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "ProcessorComponent.h"

//==============================================================================

ProcessorComponent::ProcessorComponent (foleys::ProcessorController& controllerToUse)
  : controller (controllerToUse)
{
    active.setClickingTogglesState (true);
    active.setToggleState (controller.isActive(), dontSendNotification);
    addAndMakeVisible (active);
    active.onStateChange = [&]
    {
        controller.setActive (active.getToggleState());
    };
    active.setColour (TextButton::buttonOnColourId, Colours::green);

    collapse.setClickingTogglesState (true);
    addAndMakeVisible (collapse);
    collapse.onStateChange = [&]
    {
        sendChangeMessage();
    };

    for (auto& parameter : controller.getParameters())
    {
        auto component = std::make_unique<ParameterComponent>(controller.getOwningClipDescriptor(), *parameter);
        addAndMakeVisible (component.get());
        parameterComponents.push_back (std::move (component));
    }

    controller.getOwningClipDescriptor().getOwningClip().addTimecodeListener (this);
}

ProcessorComponent::~ProcessorComponent()
{
    controller.getOwningClipDescriptor().getOwningClip().removeTimecodeListener (this);
}

void ProcessorComponent::paint (Graphics& g)
{
    g.setColour (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 6.0);

    g.setColour (Colours::grey);
    g.drawRoundedRectangle (getLocalBounds().toFloat(), 6.0, 1);

    g.setColour (Colours::silver);
    g.setFont (16.0f);

    auto area = getLocalBounds();

    g.drawText (controller.getName(), area.removeFromTop (24).reduced (30, 3),
                Justification::left, true);
}

void ProcessorComponent::resized()
{
    auto area = getLocalBounds().reduced (3);
    auto heading = area.removeFromTop (24);
    active.setBounds (heading.removeFromLeft (24));
    collapse.setBounds (heading.removeFromRight (24));

    auto collapsed = collapse.getToggleState();

    for (auto& c : parameterComponents)
    {
        c->setVisible (! collapsed);
        if (! collapse.getToggleState())
            c->setBounds (area.removeFromTop (40));
    }
}

int ProcessorComponent::getHeightForWidth(int width) const
{
    // todo: adapt to width
    ignoreUnused (width);

    if (collapse.getToggleState())
        return 30;

    return int (30 + 40 * controller.getNumParameters());
}

void ProcessorComponent::timecodeChanged (int64_t count, double seconds)
{
    auto localTime = controller.getOwningClipDescriptor().getClipTimeInDescriptorTime (seconds);
    for (auto& c : parameterComponents)
        c->updateForTime (localTime);
}

const foleys::ProcessorController* ProcessorComponent::getProcessorController() const
{
    return &controller;
}

//==============================================================================

ProcessorComponent::ParameterComponent::ParameterComponent (foleys::ClipDescriptor& clipToControl,
                                                            foleys::ParameterAutomation& parameterToControl)
  : clip (clipToControl),
    parameter (parameterToControl)
{
    prev.setConnectedEdges (TextButton::ConnectedOnRight);
    next.setConnectedEdges (TextButton::ConnectedOnRight | TextButton::ConnectedOnLeft);
    add.setConnectedEdges (TextButton::ConnectedOnLeft);
    addAndMakeVisible (valueSlider);
    addAndMakeVisible (prev);
    addAndMakeVisible (next);
    addAndMakeVisible (add);

    auto numSteps = parameter.getNumSteps();
    if (numSteps > 0)
        valueSlider.setRange (0.0, 1.0, 1.0 / parameter.getNumSteps());
    else
        valueSlider.setRange (0.0, 1.0);

    valueSlider.onDragStart = [this]
    {
        parameter.startAutomationGesture();
        dragging = true;
    };

    valueSlider.onDragEnd = [this]
    {
        dragging = false;
        parameter.finishAutomationGesture();
    };

    valueSlider.onValueChange = [this]
    {
        if (dragging)
        {
            parameter.setValue (clip.getCurrentPTS(), valueSlider.getValue());
            clip.getOwningClip().invalidateVideo();
        }
    };

    valueSlider.textFromValueFunction = [this](double value) { return parameter.getText (value); };
    valueSlider.valueFromTextFunction = [this](String text) { return parameter.getValueForText (text); };

    add.onClick = [this]
    {
        parameter.addKeyframe (clip.getCurrentPTS(), valueSlider.getValue());
    };
}

void ProcessorComponent::ParameterComponent::paint (Graphics& g)
{
    auto area = getLocalBounds().reduced (3);
    g.setColour (Colours::silver);
    g.drawFittedText (parameter.getName(), area, Justification::topLeft, 1);
}

void ProcessorComponent::ParameterComponent::resized()
{
    auto area = getLocalBounds().reduced (3);
    add.setBounds (area.removeFromRight (24).withTop (area.getHeight() - 24));
    next.setBounds (area.removeFromRight (24).withTop (area.getHeight() - 24));
    prev.setBounds (area.removeFromRight (24).withTop (area.getHeight() - 24));
    valueSlider.setBounds (area.withTop (20).withTrimmedRight (3));
}

void ProcessorComponent::ParameterComponent::updateForTime (double pts)
{
    if (dragging == false)
        valueSlider.setValue (parameter.getValueForTime (pts));
}
