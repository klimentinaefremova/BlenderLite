#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

namespace Constants {
    constexpr unsigned int SCR_WIDTH = 1000;
    constexpr unsigned int SCR_HEIGHT = 600;
    constexpr float LEFT_BAR_WIDTH = 220.0f; // Changed from MIN_LEFT_BAR_WIDTH to fixed width
    constexpr float RIGHT_BAR_WIDTH = 280.0f; // Changed from MIN_RIGHT_BAR_WIDTH to fixed width
    constexpr float MIN_WINDOW_WIDTH = 650.0f;
    constexpr float TOP_BAR_HEIGHT = 110.0f;
    constexpr double PI_DOUBLE = 3.14159265358979323846;
    constexpr float PI = static_cast<float>(PI_DOUBLE);

    // Color constants for the color buttons
    constexpr float COLOR_BLACK[3] = {0.0f, 0.0f, 0.0f};
    constexpr float COLOR_WHITE[3] = {1.0f, 1.0f, 1.0f};
    constexpr float COLOR_RED[3] = {1.0f, 0.0f, 0.0f};
    constexpr float COLOR_GREEN[3] = {0.0f, 1.0f, 0.0f};
    constexpr float COLOR_BLUE[3] = {0.0f, 0.0f, 1.0f};
    constexpr float COLOR_MAGENTA[3] = {1.0f, 0.0f, 1.0f};
}

#endif