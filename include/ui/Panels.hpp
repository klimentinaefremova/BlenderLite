#ifndef PANELS_HPP
#define PANELS_HPP

#include <string>
#include "core/ApplicationState.hpp"

class Panels {
public:
    static void drawTransformPanel(float panelX1, float panelY1, float panelWidth, float panelHeight,
                                  const std::string& title, float values[3],
                                  ApplicationState& state, int width, int height);
    static void drawShapesPanel(float panelX1, float panelY1, float panelWidth, float panelHeight,
                               ApplicationState& state, int width, int height);
    static void drawTexturesPanel(float panelX1, float panelY1, float panelWidth, float panelHeight,
                                 ApplicationState& state, int width, int height);
    static void drawTopBarUI(int width, int height, ApplicationState& appState);
};

#endif