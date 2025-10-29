#ifndef APPLICATION_STATE_HPP
#define APPLICATION_STATE_HPP

#include "Constants.hpp"
#include <string>

enum class ShapeType {
    NONE,
    CUBE,
    SPHERE,
    CONE,
    CYLINDER,
    TORUS,
    PYRAMID
};

// New struct to track active text input field
struct ActiveInputField {
    int panelType; // 0: Rotation, 1: Scaling, 2: Translation
    int axis; // 0: X, 1: Y, 2: Z
    bool active;
    std::string text;

    ActiveInputField() : panelType(-1), axis(-1), active(false), text("") {}
};

struct ApplicationState {
    double mouseX = 0.0, mouseY = 0.0;
    float translate[3] = {0.0f, 0.0f, 0.0f};
    float scale[3] = {1.0f, 1.0f, 1.0f};
    float rotate[3] = {0.0f, 0.0f, 0.0f};
    float rightPanelScroll = 0.0f;
    float leftPanelShapesScroll = 0.0f;
    float leftPanelTexturesScroll = 0.0f;

    // Updated fields for axis button selection and dragging
    bool axisHovered[3] = {false, false, false}; // X, Y, Z button hover states
    bool axisSelected[3] = {false, false, false}; // X, Y, Z selection states (can have multiple)
    bool draggingAxis = false; // True when dragging selected axes
    double dragStartX = 0.0, dragStartY = 0.0; // Drag start position

    // New field to track mouse clicks for UI interactions
    bool mouseClicked = false;

    // New field for text input
    ActiveInputField activeInputField;

    // Shape selection
    ShapeType currentShape = ShapeType::NONE;

    // New field for current shape color
    float currentColor[3] = {0.8f, 0.8f, 0.8f}; // Default light gray
};

#endif