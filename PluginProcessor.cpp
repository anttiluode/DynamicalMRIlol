#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout TakensProcessor::createLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>("input_gain", "Gain", juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.5f), 1.0f));
    return layout;
}

TakensProcessor::TakensProcessor() : AudioProcessor(BusesProperties()
    .withInput("Input", juce::AudioChannelSet::stereo(), true)
    .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    apvts(*this, nullptr, "PARAMS", createLayout()) {}

void TakensProcessor::prepareToPlay(double, int) {
    ringBufferL.fill(0.0f);
    ringBufferR.fill(0.0f);
    writeIndex.store(0);
    rmsLevel.store(0.0f);
}

void TakensProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
    const float gain = apvts.getRawParameterValue("input_gain")->load();
    const int numSamples = buffer.getNumSamples();
    const int numCh = buffer.getNumChannels();
    const float* dataL = buffer.getReadPointer(0);
    const float* dataR = (numCh > 1) ? buffer.getReadPointer(1) : dataL;

    int idx = writeIndex.load(std::memory_order_relaxed);
    float rmsSum = 0.0f;

    for (int s = 0; s < numSamples; ++s) {
        const float L = dataL[s] * gain;
        const float R = dataR[s] * gain;
        ringBufferL[idx] = L;
        ringBufferR[idx] = R;
        idx = (idx + 1) % bufferSize;
        rmsSum += (L * L + R * R) * 0.5f;
    }

    writeIndex.store(idx, std::memory_order_relaxed);
    if (numSamples > 0) rmsLevel.store(std::sqrt(rmsSum / (float)numSamples));

    // Update Dynamics Engine Once Per Block
    dynamics.process(ringBufferL, ringBufferR, idx);
}

juce::AudioProcessorEditor* TakensProcessor::createEditor() { return new TakensEditor(*this); }
void TakensProcessor::getStateInformation(juce::MemoryBlock& dest) {
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, dest);
}
void TakensProcessor::setStateInformation(const void* data, int size) {
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, size));
    if (xml && xml->hasTagName(apvts.state.getType())) apvts.replaceState(juce::ValueTree::fromXml(*xml));
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new TakensProcessor(); }