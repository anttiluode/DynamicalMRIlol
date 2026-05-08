#include "MultiTauEmbedding.h"
#include <cmath>

void MultiTauEmbedding::process(const std::array<float, 16384>& L, const std::array<float, 16384>& R, int writeIndex) {
    layers.clear();
    constexpr int N = 4000;

    for (auto tau : taus) {
        TauLayer layer;
        layer.tau = tau;

        float coherenceAccum = 0.0f;
        float energyAccum = 0.0f;

        for (int i = 0; i < N; ++i) {
            int idx0 = (writeIndex - i + 16384) % 16384;
            int idx1 = (idx0 - tau + 16384) % 16384;
            int idx2 = (idx0 - tau * 2 + 16384) % 16384;

            float x = L[idx0];
            float y = L[idx1];
            float z = L[idx2]; // Simplified Mono Mode for baseline

            layer.points.push_back({x, y, z});

            coherenceAccum += x * y;
            energyAccum += x * x + y * y + z * z;
        }

        layer.coherence = coherenceAccum / (float)N;
        layer.energy = std::sqrt(energyAccum / (float)N);
        layers.push_back(layer);
    }
}

const std::vector<TauLayer>& MultiTauEmbedding::getLayers() const {
    return layers;
}