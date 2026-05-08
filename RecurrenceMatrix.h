#pragma once
#include "MultiTauEmbedding.h"
#include <vector>

class RecurrenceMatrix {
public:
    void compute(const TauLayer& layer);
    const std::vector<float>& getMatrix() const;
    int getSize() const { return size; }

private:
    std::vector<float> matrix;
    int size = 128;
};