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
    double lastX = 0.0, lastY = 0.0;
    bool isDragging = false;

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

    // FIXED: Add missing currentColor field
    float currentColor[3] = {0.8f, 0.8f, 0.8f}; // Default light gray

    // FIXED: Rename currentTexture to selectedTexture for consistency
    int selectedTexture = -1; // -1 means no texture selected

    // NEW: Store texture for each shape and track if using texture or color
    int shapeTextures[6] = {-1, -1, -1, -1, -1, -1}; // Texture IDs for each shape type
    bool shapeUsesTexture[6] = {false, false, false, false, false, false}; // Track if shape uses texture or color

    // NEW: Default colors for each shape type
    float shapeColors[6][3] = {
        {0.8f, 0.8f, 0.8f}, // Cube - light gray
        {0.8f, 0.8f, 0.8f}, // Sphere - light gray
        {0.8f, 0.8f, 0.8f}, // Cone - light gray
        {0.8f, 0.8f, 0.8f}, // Cylinder - light gray
        {0.8f, 0.8f, 0.8f}, // Torus - light gray
        {0.8f, 0.8f, 0.8f}  // Pyramid - light gray
    };
};

#endif