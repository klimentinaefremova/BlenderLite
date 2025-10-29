#ifndef TRANSLATION_LIMITS_HPP
#define TRANSLATION_LIMITS_HPP

#include <algorithm> // for std::min and std::max
#include <cmath>     // for std::floor

// Base translation limits (when Z = -0.400)
constexpr float BASE_X_MIN = -0.350f;
constexpr float BASE_X_MAX = 0.200f;
constexpr float BASE_Y_MIN = -0.700f;
constexpr float BASE_Y_MAX = 0.350f;
constexpr float Z_MIN = -1000.0f; // -infinity approximation
constexpr float Z_MAX = -0.400f;

// Border adjustment parameters
constexpr float Z_REFERENCE = -0.400f; // Reference Z value for base borders
constexpr float Z_CHANGE_FOR_ADJUSTMENT = 0.300f; // Z change required for each adjustment step

// Function to calculate current borders based on Z position
inline void getCurrentBorders(float z, float& xMin, float& xMax, float& yMin, float& yMax) {
    // Calculate how much Z has changed from reference (negative means Z decreased)
    float zChange = z - Z_REFERENCE;

    // Calculate the number of full adjustment steps (each 0.300 Z decrease)
    int adjustmentSteps = static_cast<int>(std::floor(-zChange / Z_CHANGE_FOR_ADJUSTMENT));

    // Ensure at least 0 steps
    adjustmentSteps = std::max(0, adjustmentSteps);

    // Calculate the adjustment for this specific step
    // Step 1: 0.05, Step 2: 0.10, Step 3: 0.15, Step 4: 0.20, etc.
    float stepAdjustment = adjustmentSteps * 0.05f;

    // Apply adjustment to borders (expand borders as Z decreases)
    xMin = BASE_X_MIN - stepAdjustment;
    xMax = BASE_X_MAX + stepAdjustment;
    yMin = BASE_Y_MIN - stepAdjustment;
    yMax = BASE_Y_MAX + stepAdjustment;

    // Debug output to see the adjustment in action
    // std::cout << "Z: " << z << " | Steps: " << adjustmentSteps
    //           << " | Step Adjustment: " << stepAdjustment
    //           << " | X: [" << xMin << ", " << xMax << "]"
    //           << " | Y: [" << yMin << ", " << yMax << "]" << std::endl;
}

// Function to apply translation limits with dynamic borders
inline void applyTranslationLimits(float translate[3]) {
    float currentXMin, currentXMax, currentYMin, currentYMax;
    getCurrentBorders(translate[2], currentXMin, currentXMax, currentYMin, currentYMax);

    // Apply X limits with dynamic borders
    translate[0] = std::max(currentXMin, std::min(currentXMax, translate[0]));

    // Apply Y limits with dynamic borders
    translate[1] = std::max(currentYMin, std::min(currentYMax, translate[1]));

    // Apply Z limits: [-infinity, -0.400]
    translate[2] = std::min(Z_MAX, translate[2]); // Only upper limit for Z
    // No lower limit for Z (can go to -infinity)
}

#endif