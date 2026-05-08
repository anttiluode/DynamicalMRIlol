#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <deque>

class TakensEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
    explicit TakensEditor(TakensProcessor&);
    ~TakensEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;

private:
    void drawProjectedPoint(juce::Graphics& g, EmbeddedPoint p, juce::Colour col);
    void drawRecurrenceMatrix(juce::Graphics& g, const std::vector<float>& mat, int size, juce::Rectangle<int> area);

    TakensProcessor& processor;
    
    float rotX = 0.38f, rotY = 0.0f;
    juce::Point<int> lastMousePos;
    
    std::vector<DynamicLayerState> currentStates;
    std::deque<std::vector<EmbeddedPoint>> history;

    juce::Label hudLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TakensEditor)
};