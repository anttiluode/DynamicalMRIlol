#include "PluginEditor.h"

static const juce::Colour COL_BG { 0xff0a0a1a };
static const juce::Colour COL_PANEL { 0xff0d0d20 };

TakensEditor::TakensEditor(TakensProcessor& p) : AudioProcessorEditor(&p), processor(p) {
    setSize(1000, 700);
    setResizable(true, true);
    setResizeLimits(800, 600, 1920, 1080);
    
    hudLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(hudLabel);
    
    startTimerHz(30);
}

TakensEditor::~TakensEditor() {}

void TakensEditor::timerCallback() {
    currentStates = processor.dynamics.getStatesSafe();
    
    if (!currentStates.empty()) {
        history.push_front(currentStates[0].embedding.points);
        while (history.size() > 8) {
            history.pop_back();
        }
    }
    repaint();
}

void TakensEditor::mouseDown(const juce::MouseEvent& e) { lastMousePos = e.getPosition(); }
void TakensEditor::mouseDrag(const juce::MouseEvent& e) {
    auto delta = e.getPosition() - lastMousePos;
    rotY += delta.x * 0.01f;
    rotX += delta.y * 0.01f;
    lastMousePos = e.getPosition();
}

void TakensEditor::drawProjectedPoint(juce::Graphics& g, EmbeddedPoint p, juce::Colour col) {
    float x1 = p.x * std::cos(rotY) - p.z * std::sin(rotY);
    float z1 = p.x * std::sin(rotY) + p.z * std::cos(rotY);
    float y1 = p.y * std::cos(rotX) - z1 * std::sin(rotX);
    float z2 = p.y * std::sin(rotX) + z1 * std::cos(rotX);

    float cameraDistance = 2.5f;
    float w = 1.0f / (cameraDistance - z2);
    float scale = 350.0f;

    float sx = getWidth() * 0.5f + x1 * w * scale;
    float sy = getHeight() * 0.5f - y1 * w * scale;
    float size = juce::jmax(1.0f, 8.0f * w);

    g.setColour(col.withMultipliedAlpha(juce::jlimit(0.1f, 1.0f, w)));
    g.fillEllipse(sx - size * 0.5f, sy - size * 0.5f, size, size);
}

void TakensEditor::drawRecurrenceMatrix(juce::Graphics& g, const std::vector<float>& mat, int size, juce::Rectangle<int> area) {
    if (mat.empty() || size == 0) return;
    
    float cellW = (float)area.getWidth() / size;
    float cellH = (float)area.getHeight() / size;

    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            float v = mat[y * size + x];
            juce::uint8 c = (juce::uint8)(v * 255.0f);
            g.setColour(juce::Colour(c, c, c));
            g.fillRect(area.getX() + x * cellW, area.getY() + y * cellH, cellW + 1.0f, cellH + 1.0f);
        }
    }
}

void TakensEditor::paint(juce::Graphics& g) {
    g.fillAll(COL_BG);
    
    if (currentStates.empty()) return;

    // 1. Draw Recurrence Matrix
    drawRecurrenceMatrix(g, currentStates[0].recurrence, 128, juce::Rectangle<int>(getWidth() - 270, 20, 250, 250));

    // 2. HUD text based on the primary state
    auto& primary = currentStates[0];
    juce::String topoName = TopologyAnalyzer::classifyTopology(primary.topology);
    
    g.setFont(16.0f);
    g.setColour(juce::Colours::white);
    g.drawFittedText("TOPOLOGY: " + topoName, 20, 20, 300, 30, juce::Justification::left, 1);
    g.drawFittedText("CHAOS: " + juce::String(primary.smoothedChaos, 3), 20, 50, 300, 30, juce::Justification::left, 1);
    g.drawFittedText("COHERENCE: " + juce::String(primary.smoothedCoherence, 3), 20, 80, 300, 30, juce::Justification::left, 1);
    g.drawFittedText("VOLUME: " + juce::String(primary.smoothedVolume, 4), 20, 110, 300, 30, juce::Justification::left, 1);

    // 3. Render Temporal Layers (Multi-Tau)
    for (size_t layerIdx = 0; layerIdx < currentStates.size(); ++layerIdx) {
        const auto& state = currentStates[layerIdx];
        float layerDepth = (float)layerIdx * 0.4f;

        for (size_t i = 0; i < state.embedding.points.size(); ++i) {
            auto p = state.embedding.points[i];
            p.z += layerDepth; // Offset layers in depth

            float chaos = state.smoothedChaos;
            juce::Colour col;

            if (chaos < 0.08f)      col = juce::Colours::cyan;
            else if (chaos < 0.2f)  col = juce::Colours::green;
            else if (chaos < 0.4f)  col = juce::Colours::yellow;
            else                    col = juce::Colours::red;

            drawProjectedPoint(g, p, col);
        }
    }

    // 4. Render History Trails (Fading memory of primary attractor)
    for (size_t histIdx = 0; histIdx < history.size(); ++histIdx) {
        float fade = 1.0f - ((float)histIdx / (float)history.size());
        auto& pts = history[histIdx];
        
        for (const auto& p : pts) {
            drawProjectedPoint(g, p, juce::Colours::cyan.withAlpha(fade * 0.3f));
        }
    }
}

void TakensEditor::resized() {
    hudLabel.setBounds(20, 20, 300, 100);
}