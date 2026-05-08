#include "RecurrenceMatrix.h"
#include <cmath>
#include <algorithm>

void RecurrenceMatrix::compute(const TauLayer& layer) {
    matrix.resize(size * size, 0.0f);
    int safeSize = std::min(size, (int)layer.points.size());

    for (int y = 0; y < safeSize; ++y) {
        for (int x = 0; x < safeSize; ++x) {
            const auto& a = layer.points[y];
            const auto& b = layer.points[x];

            float dx = a.x - b.x;
            float dy = a.y - b.y;
            float dz = a.z - b.z;

            float d = std::sqrt(dx*dx + dy*dy + dz*dz);
            matrix[y * size + x] = std::exp(-d * 8.0f);
        }
    }
}

const std::vector<float>& RecurrenceMatrix::getMatrix() const {
    return matrix;
}