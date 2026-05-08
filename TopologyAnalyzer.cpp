#include "TopologyAnalyzer.h"
#include <cmath>
#include <algorithm>

TopologyState TopologyAnalyzer::analyze(const TauLayer& layer) {
    TopologyState s;
    const auto& pts = layer.points;

    if (pts.empty()) return s;

    float minX = 9999.f, minY = 9999.f, minZ = 9999.f;
    float maxX = -9999.f, maxY = -9999.f, maxZ = -9999.f;

    for (const auto& p : pts) {
        minX = std::min(minX, p.x); minY = std::min(minY, p.y); minZ = std::min(minZ, p.z);
        maxX = std::max(maxX, p.x); maxY = std::max(maxY, p.y); maxZ = std::max(maxZ, p.z);
    }

    s.attractorVolume = (maxX - minX) * (maxY - minY) * (maxZ - minZ);
    s.curvature = computeCurvature(pts);
    s.chaos = computeDivergence(pts);

    s.torusLike = (s.curvature < 0.15f && s.chaos < 0.10f);
    s.chaotic = (s.chaos > 0.4f);

    return s;
}

juce::String TopologyAnalyzer::classifyTopology(const TopologyState& s) {
    if (s.torusLike) return "TORUS";
    if (s.chaotic) return "CHAOTIC";
    if (s.attractorVolume < 0.005f) return "COLLAPSED";
    return "HYBRID";
}

float TopologyAnalyzer::computeCurvature(const std::vector<EmbeddedPoint>& pts) {
    if (pts.size() < 3) return 0.0f;
    float accum = 0.0f;

    for (size_t i = 1; i < pts.size() - 1; ++i) {
        auto a = pts[i-1], b = pts[i], c = pts[i+1];
        float vx1 = b.x - a.x, vy1 = b.y - a.y, vz1 = b.z - a.z;
        float vx2 = c.x - b.x, vy2 = c.y - b.y, vz2 = c.z - b.z;
        float dot = vx1*vx2 + vy1*vy2 + vz1*vz2;
        accum += 1.0f - dot;
    }
    return accum / (float)pts.size();
}

float TopologyAnalyzer::computeDivergence(const std::vector<EmbeddedPoint>& pts) {
    if (pts.size() < 2) return 0.0f;
    float accum = 0.0f;

    for (size_t i = 1; i < pts.size(); ++i) {
        float dx = pts[i].x - pts[i-1].x;
        float dy = pts[i].y - pts[i-1].y;
        float dz = pts[i].z - pts[i-1].z;
        accum += std::sqrt(dx*dx + dy*dy + dz*dz);
    }
    return accum / (float)pts.size();
}