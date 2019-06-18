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

    ProcessorComponent.h
    Created: 25 May 2019 11:18:43am
    Author:  Daniel Walz

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class ProcessorComponent    : public Component,
                              public ChangeBroadcaster,
                              private foleys::AVClip::TimecodeListener
{
public:
    ProcessorComponent (foleys::ProcessorController& controller);
    ~ProcessorComponent();

    void paint (Graphics&) override;
    void resized() override;

    void showProcessorEditor (AudioProcessorEditor* editor, const String& title);

    int getHeightForWidth(int width) const;

    void timecodeChanged (int64_t count, double seconds) override;

    const foleys::ProcessorController* getProcessorController() const;

    class ParameterComponent : public juce::Component, private foleys::ProcessorParameter::Listener
    {
    public:
        ParameterComponent (foleys::ClipDescriptor& clip, foleys::ParameterAutomation& parameter);

        void paint (Graphics&) override;
        void resized() override;

        void valueChanged (foleys::ProcessorParameter& parameter, double value) override {}
        void gestureStarted (foleys::ProcessorParameter& parameter) override {}
        void gestureFinished (foleys::ProcessorParameter& parameter) override {}

        void updateForTime (double pts);

    private:
        foleys::ClipDescriptor& clip;
        foleys::ParameterAutomation& parameter;
        bool dragging = false;
        Slider valueSlider { Slider::LinearHorizontal, Slider::TextBoxRight };
        TextButton prev { "<" };
        TextButton next { ">" };
        TextButton add  { "+" };
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterComponent)
    };
private:

    bool isCollapsed() const;

    TextButton active   { "A" };
    TextButton editor   { "E" };
    TextButton collapse { "v" };
    TextButton remove   { "X" };

    foleys::ProcessorController& controller;
    std::vector<std::unique_ptr<ParameterComponent>> parameterComponents;

    //==============================================================================

    class AudioProcessorWindow  : public DocumentWindow
    {
    public:
        AudioProcessorWindow (AudioProcessorEditor* editor, const String& title);
        void closeButtonPressed() override;
    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioProcessorWindow)
    };
    std::unique_ptr<AudioProcessorWindow> audioProcessorWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProcessorComponent)
};
