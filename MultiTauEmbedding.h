#pragma once
#include <vector>
#include <array>

struct EmbeddedPoint {
    float x, y, z;
};

struct TauLayer {
    int tau;
    std::vector<EmbeddedPoint> points;
    float coherence = 0.0f;
    float energy = 0.0f;
    float divergence = 0.0f;
};

class MultiTauEmbedding {
public:
    void process(const std::array<float, 16384>& L, const std::array<float, 16384>& R, int writeIndex);
    const std::vector<TauLayer>& getLayers() const;

private:
    std::vector<TauLayer> layers;
    std::vector<int> taus = { 2, 8, 24, 72 };
};