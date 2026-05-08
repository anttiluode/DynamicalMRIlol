#pragma once
#include "MultiTauEmbedding.h"
#include "TopologyAnalyzer.h"
#include "RecurrenceMatrix.h"
#include <mutex>

struct DynamicLayerState {
    TauLayer embedding;
    TopologyState topology;
    std::vector<float> recurrence;

    float smoothedChaos = 0.0f;
    float smoothedCoherence = 0.0f;
    float smoothedVolume = 0.0f;
};

class DynamicsEngine {
public:
    void process(const std::array<float, 16384>& L, const std::array<float, 16384>& R, int writeIndex);
    const std::vector<DynamicLayerState> getStatesSafe(); 

private:
    MultiTauEmbedding embedder;
    TopologyAnalyzer analyzer;
    RecurrenceMatrix recurrence;
    std::vector<DynamicLayerState> states;
    std::mutex stateMutex;

    float smooth(float oldV, float newV, float amt);
};