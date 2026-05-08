#pragma once
#include "MultiTauEmbedding.h"
#include <JuceHeader.h>

struct TopologyState {
    float attractorVolume = 0.0f;
    float curvature = 0.0f;
    float chaos = 0.0f;

    bool torusLike = false;
    bool chaotic = false;
};

class TopologyAnalyzer {
public:
    TopologyState analyze(const TauLayer& layer);
    static juce::String classifyTopology(const TopologyState& s);

private:
    float computeCurvature(const std::vector<EmbeddedPoint>& pts);
    float computeDivergence(const std::vector<EmbeddedPoint>& pts);
};