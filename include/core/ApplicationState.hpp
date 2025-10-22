#ifndef APPLICATION_STATE_HPP
#define APPLICATION_STATE_HPP

#include "Constants.hpp"

struct ApplicationState {
    float leftBarWidth = Constants::INITIAL_LEFT_BAR_WIDTH;
    float rightBarWidth = Constants::INITIAL_RIGHT_BAR_WIDTH;
    bool resizingLeft = false;
    bool resizingRight = false;
    double lastX = 0.0, lastY = 0.0;
    double mouseX = 0.0, mouseY = 0.0;
    float translate[3] = {0.0f, 0.0f, 0.0f};
    float scale[3] = {1.0f, 1.0f, 1.0f};
    float rotate[3] = {0.0f, 0.0f, 0.0f};
    float rightPanelScroll = 0.0f;
    float leftPanelShapesScroll = 0.0f;
    float leftPanelTexturesScroll = 0.0f;
};

#endif