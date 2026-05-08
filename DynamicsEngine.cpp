#include "DynamicsEngine.h"

float DynamicsEngine::smooth(float oldV, float newV, float amt) {
    return oldV + (newV - oldV) * amt;
}

void DynamicsEngine::process(const std::array<float, 16384>& L, const std::array<float, 16384>& R, int writeIndex) {
    embedder.process(L, R, writeIndex);
    auto& layers = embedder.getLayers();

    std::lock_guard<std::mutex> lock(stateMutex);

    if (states.size() != layers.size())
        states.resize(layers.size());

    for (size_t i = 0; i < layers.size(); ++i) {
        auto topo = analyzer.analyze(layers[i]);
        if (i == 0) recurrence.compute(layers[i]); // Compute recurrence for primary tau layer to save CPU

        auto& s = states[i];
        s.embedding = layers[i];
        s.topology = topo;
        if (i == 0) s.recurrence = recurrence.getMatrix();

        s.smoothedChaos = smooth(s.smoothedChaos, topo.chaos, 0.05f);
        s.smoothedCoherence = smooth(s.smoothedCoherence, layers[i].coherence, 0.05f);
        s.smoothedVolume = smooth(s.smoothedVolume, topo.attractorVolume, 0.05f);
    }
}

const std::vector<DynamicLayerState> DynamicsEngine::getStatesSafe() {
    std::lock_guard<std::mutex> lock(stateMutex);
    return states;
}