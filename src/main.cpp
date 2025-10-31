#include "BlenderLite.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <string>
#include <sstream>
#include <iomanip>

ApplicationState appState;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void handleTextInput(GLFWwindow* window, int key, int scancode, int action, int mods);
void handleCharacterInput(GLFWwindow* window, unsigned int codepoint);

// Enhanced 3D shape drawing functions with texture mapping
void drawCube(ApplicationState& appState) {
    glBegin(GL_QUADS);

    // Front face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);

    // Back face
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);

    // Top face
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);

    // Bottom face
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);

    // Right face
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);

    // Left face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);

    glEnd();
}

void drawSphere(float radius, int slices, int stacks, ApplicationState& appState) {
    for (int i = 0; i <= stacks; ++i) {
        float phi = Constants::PI * i / stacks;
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float theta = 2.0f * Constants::PI * j / slices;
            float x = radius * sin(phi) * cos(theta);
            float y = radius * cos(phi);
            float z = radius * sin(phi) * sin(theta);

            // Texture coordinates based on spherical mapping
            float s = (float)j / slices;
            float t = (float)i / stacks;
            glTexCoord2f(s, t);
            glVertex3f(x, y, z);

            float x2 = radius * sin(phi + Constants::PI / stacks) * cos(theta);
            float y2 = radius * cos(phi + Constants::PI / stacks);
            float z2 = radius * sin(phi + Constants::PI / stacks) * sin(theta);

            float t2 = (float)(i + 1) / stacks;
            glTexCoord2f(s, t2);
            glVertex3f(x2, y2, z2);
        }
        glEnd();
    }
}

void drawCone(float base, float height, int slices, ApplicationState& appState) {
    // Base
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, -height/2, 0.0f);
    for (int i = 0; i <= slices; ++i) {
        float theta = 2.0f * Constants::PI * i / slices;
        float x = base * cos(theta);
        float z = base * sin(theta);
        float s = 0.5f + 0.5f * cos(theta);
        float t = 0.5f + 0.5f * sin(theta);
        glTexCoord2f(s, t);
        glVertex3f(x, -height/2, z);
    }
    glEnd();

    // Side
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.5f, 1.0f);
    glVertex3f(0.0f, height/2, 0.0f);
    for (int i = 0; i <= slices; ++i) {
        float theta = 2.0f * Constants::PI * i / slices;
        float x = base * cos(theta);
        float z = base * sin(theta);
        float s = (float)i / slices;
        glTexCoord2f(s, 0.0f);
        glVertex3f(x, -height/2, z);
    }
    glEnd();
}

void drawCylinder(float radius, float height, int slices, ApplicationState& appState) {
    // Top
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, height/2, 0.0f);
    for (int i = 0; i <= slices; ++i) {
        float theta = 2.0f * Constants::PI * i / slices;
        float x = radius * cos(theta);
        float z = radius * sin(theta);
        float s = 0.5f + 0.5f * cos(theta);
        float t = 0.5f + 0.5f * sin(theta);
        glTexCoord2f(s, t);
        glVertex3f(x, height/2, z);
    }
    glEnd();

    // Bottom
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, -height/2, 0.0f);
    for (int i = 0; i <= slices; ++i) {
        float theta = 2.0f * Constants::PI * i / slices;
        float x = radius * cos(theta);
        float z = radius * sin(theta);
        float s = 0.5f + 0.5f * cos(theta);
        float t = 0.5f + 0.5f * sin(theta);
        glTexCoord2f(s, t);
        glVertex3f(x, -height/2, z);
    }
    glEnd();

    // Side
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; ++i) {
        float theta = 2.0f * Constants::PI * i / slices;
        float x = radius * cos(theta);
        float z = radius * sin(theta);
        float s = (float)i / slices;

        glTexCoord2f(s, 1.0f);
        glVertex3f(x, height/2, z);
        glTexCoord2f(s, 0.0f);
        glVertex3f(x, -height/2, z);
    }
    glEnd();
}

void drawTorus(float majorRadius, float minorRadius, int majorSlices, int minorSlices, ApplicationState& appState) {
    for (int i = 0; i < majorSlices; ++i) {
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= minorSlices; ++j) {
            for (int k = 0; k <= 1; ++k) {
                float phi = 2.0f * Constants::PI * (i + k) / majorSlices;
                float theta = 2.0f * Constants::PI * j / minorSlices;

                float x = (majorRadius + minorRadius * cos(theta)) * cos(phi);
                float y = minorRadius * sin(theta);
                float z = (majorRadius + minorRadius * cos(theta)) * sin(phi);

                // Texture coordinates
                float s = (float)(i + k) / majorSlices;
                float t = (float)j / minorSlices;
                glTexCoord2f(s, t);
                glVertex3f(x, y, z);
            }
        }
        glEnd();
    }
}

void drawPyramid(float base, float height, ApplicationState& appState) {
    // Base
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-base/2, -height/2, -base/2);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(base/2, -height/2, -base/2);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(base/2, -height/2, base/2);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-base/2, -height/2, base/2);
    glEnd();

    // Front face
    glBegin(GL_TRIANGLES);
    glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, height/2, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-base/2, -height/2, base/2);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(base/2, -height/2, base/2);
    glEnd();

    // Right face
    glBegin(GL_TRIANGLES);
    glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, height/2, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(base/2, -height/2, base/2);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(base/2, -height/2, -base/2);
    glEnd();

    // Back face
    glBegin(GL_TRIANGLES);
    glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, height/2, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(base/2, -height/2, -base/2);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-base/2, -height/2, -base/2);
    glEnd();

    // Left face
    glBegin(GL_TRIANGLES);
    glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, height/2, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-base/2, -height/2, -base/2);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-base/2, -height/2, base/2);
    glEnd();
}

void drawCurrentShape(ApplicationState& appState) {
    if (appState.currentShape == ShapeType::NONE) {
        return;
    }

    // Enable depth testing for 3D
    glEnable(GL_DEPTH_TEST);

    // Set up projection matrix
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    // Manual perspective projection matrix
    float aspect = 1.0f;
    float fov = 45.0f * Constants::PI / 180.0f;
    float close = 0.1f;
    float faar = 100.0f;

    float f = 1.0f / tan(fov / 2.0f);
    float range = close - faar;

    float projection[16] = {
        f / aspect, 0, 0, 0,
        0, f, 0, 0,
        0, 0, (faar + close) / range, -1,
        0, 0, (2 * faar * close) / range, 0
    };

    glMultMatrixf(projection);

    // Set up modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Manual camera setup
    float eyeX = 0.0f, eyeY = 0.0f, eyeZ = 3.0f;
    float centerX = 0.0f, centerY = 0.0f, centerZ = 0.0f;
    float upX = 0.0f, upY = 1.0f, upZ = 0.0f;

    float forward[3] = {
        centerX - eyeX,
        centerY - eyeY,
        centerZ - eyeZ
    };

    // Normalize forward vector
    float length = sqrt(forward[0]*forward[0] + forward[1]*forward[1] + forward[2]*forward[2]);
    forward[0] /= length;
    forward[1] /= length;
    forward[2] /= length;

    // Right = forward x up
    float right[3] = {
        forward[1] * upZ - forward[2] * upY,
        forward[2] * upX - forward[0] * upZ,
        forward[0] * upY - forward[1] * upX
    };

    // Normalize right vector
    length = sqrt(right[0]*right[0] + right[1]*right[1] + right[2]*right[2]);
    right[0] /= length;
    right[1] /= length;
    right[2] /= length;

    // Up = right x forward
    float actualUp[3] = {
        right[1] * forward[2] - right[2] * forward[1],
        right[2] * forward[0] - right[0] * forward[2],
        right[0] * forward[1] - right[1] * forward[0]
    };

    float view[16] = {
        right[0], actualUp[0], -forward[0], 0,
        right[1], actualUp[1], -forward[1], 0,
        right[2], actualUp[2], -forward[2], 0,
        -(right[0]*eyeX + right[1]*eyeY + right[2]*eyeZ),
        -(actualUp[0]*eyeX + actualUp[1]*eyeY + actualUp[2]*eyeZ),
        forward[0]*eyeX + forward[1]*eyeY + forward[2]*eyeZ,
        1
    };

    glMultMatrixf(view);

    // Apply transformations from appState
    glTranslatef(appState.translate[0], appState.translate[1], appState.translate[2]);
    glScalef(appState.scale[0], appState.scale[1], appState.scale[2]);
    glRotatef(appState.rotate[0], 1.0f, 0.0f, 0.0f);
    glRotatef(appState.rotate[1], 0.0f, 1.0f, 0.0f);
    glRotatef(appState.rotate[2], 0.0f, 0.0f, 1.0f);

    // NEW: Check if shape uses texture or color
    bool usesTexture = PrimitiveRenderer::shapeHasTexture(appState.currentShape, appState);

    if (usesTexture) {
        int textureID = PrimitiveRenderer::getShapeTexture(appState.currentShape, appState);

        if (textureID != -1 && textureID < PrimitiveRenderer::textureIDs.size() &&
            PrimitiveRenderer::textureIDs[textureID] != 0) {
            // Enable texturing with proper parameters
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, PrimitiveRenderer::textureIDs[textureID]);

            // Set texture parameters (important!)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // Use white to preserve texture colors
        } else {
            // Fallback to color if texture is invalid
            glDisable(GL_TEXTURE_2D);
            glColor3f(appState.currentColor[0], appState.currentColor[1], appState.currentColor[2]);
        }
    } else {
        // Use regular coloring
        glDisable(GL_TEXTURE_2D);
        glColor3f(appState.currentColor[0], appState.currentColor[1], appState.currentColor[2]);
    }

    // Draw the selected shape
    switch (appState.currentShape) {
        case ShapeType::CUBE:
            drawCube(appState);
            break;
        case ShapeType::SPHERE:
            drawSphere(0.5f, 20, 20, appState);
            break;
        case ShapeType::CONE:
            drawCone(0.5f, 1.0f, 20, appState);
            break;
        case ShapeType::CYLINDER:
            drawCylinder(0.5f, 1.0f, 20, appState);
            break;
        case ShapeType::TORUS:
            drawTorus(0.5f, 0.2f, 20, 10, appState);
            break;
        case ShapeType::PYRAMID:
            drawPyramid(1.0f, 1.0f, appState);
            break;
        default:
            break;
    }

    // Restore matrices
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    // Disable depth testing and texturing for 2D UI
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
}

// REMOVED: draw3DAxes function - we're moving the axes to be above the buttons

void drawAxisButtons(float leftEdgeNDC, float canvasY1, float canvasY2, int width, int height) {
    // Save current matrix state
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    // Use the full screen projection since we're positioning relative to leftEdgeNDC
    glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Button size and spacing
    float buttonSize = 0.035f;
    float buttonGap = 0.002f;
    float marginY = 0.03f;

    // Position buttons attached to the left sidebar's right edge
    float startX = leftEdgeNDC;
    float startY = canvasY1 + marginY;

    // X button (Red)
    float xBtnX1 = startX;
    float xBtnX2 = xBtnX1 + buttonSize;
    float xBtnY1 = startY;
    float xBtnY2 = xBtnY1 + buttonSize;

    bool hoverX = PrimitiveRenderer::isInsideNDC(appState.mouseX, appState.mouseY, width, height, xBtnX1, xBtnY1, xBtnX2, xBtnY2);

    if (appState.axisSelected[0]) {
        PrimitiveRenderer::drawRect(xBtnX1, xBtnY1, xBtnX2, xBtnY2, 1.0f, 0.5f, 0.5f);
    } else if (hoverX) {
        PrimitiveRenderer::drawRect(xBtnX1, xBtnY1, xBtnX2, xBtnY2, 1.0f, 0.8f, 0.8f);
    } else {
        PrimitiveRenderer::drawRect(xBtnX1, xBtnY1, xBtnX2, xBtnY2, 1.0f, 0.0f, 0.0f);
    }
    PrimitiveRenderer::drawOutlineRect(xBtnX1, xBtnY1, xBtnX2, xBtnY2, 0.0f, 0.0f, 0.0f, 1.5f);

    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText("X", xBtnX1 + buttonSize * 0.3f, xBtnY1 + buttonSize * 0.3f, GLUT_BITMAP_HELVETICA_10);

    // Y button (Green)
    float yBtnX1 = xBtnX2 + buttonGap;
    float yBtnX2 = yBtnX1 + buttonSize;
    float yBtnY1 = startY;
    float yBtnY2 = yBtnY1 + buttonSize;

    bool hoverY = PrimitiveRenderer::isInsideNDC(appState.mouseX, appState.mouseY, width, height, yBtnX1, yBtnY1, yBtnX2, yBtnY2);

    if (appState.axisSelected[1]) {
        PrimitiveRenderer::drawRect(yBtnX1, yBtnY1, yBtnX2, yBtnY2, 0.5f, 1.0f, 0.5f);
    } else if (hoverY) {
        PrimitiveRenderer::drawRect(yBtnX1, yBtnY1, yBtnX2, yBtnY2, 0.8f, 1.0f, 0.8f);
    } else {
        PrimitiveRenderer::drawRect(yBtnX1, yBtnY1, yBtnX2, yBtnY2, 0.0f, 1.0f, 0.0f);
    }
    PrimitiveRenderer::drawOutlineRect(yBtnX1, yBtnY1, yBtnX2, yBtnY2, 0.0f, 0.0f, 0.0f, 1.5f);

    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText("Y", yBtnX1 + buttonSize * 0.3f, yBtnY1 + buttonSize * 0.3f, GLUT_BITMAP_HELVETICA_10);

    // Z button (Blue)
    float zBtnX1 = yBtnX2 + buttonGap;
    float zBtnX2 = zBtnX1 + buttonSize;
    float zBtnY1 = startY;
    float zBtnY2 = zBtnY1 + buttonSize;

    bool hoverZ = PrimitiveRenderer::isInsideNDC(appState.mouseX, appState.mouseY, width, height, zBtnX1, zBtnY1, zBtnX2, zBtnY2);

    if (appState.axisSelected[2]) {
        PrimitiveRenderer::drawRect(zBtnX1, zBtnY1, zBtnX2, zBtnY2, 0.5f, 0.5f, 1.0f);
    } else if (hoverZ) {
        PrimitiveRenderer::drawRect(zBtnX1, zBtnY1, zBtnX2, zBtnY2, 0.8f, 0.8f, 1.0f);
    } else {
        PrimitiveRenderer::drawRect(zBtnX1, zBtnY1, zBtnX2, zBtnY2, 0.0f, 0.0f, 1.0f);
    }
    PrimitiveRenderer::drawOutlineRect(zBtnX1, zBtnY1, zBtnX2, zBtnY2, 0.0f, 0.0f, 0.0f, 1.5f);

    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText("Z", zBtnX1 + buttonSize * 0.3f, zBtnY1 + buttonSize * 0.3f, GLUT_BITMAP_HELVETICA_10);

    // Store hover state for mouse click detection
    appState.axisHovered[0] = hoverX;
    appState.axisHovered[1] = hoverY;
    appState.axisHovered[2] = hoverZ;

    // NEW: Draw vector arrows ABOVE the buttons (moved from main canvas)
    float arrowY = startY + buttonSize + 0.015f; // Position above buttons
    float axisLength = 0.02f; // Much smaller arrows
    float arrowSize = 0.005f; // Smaller arrow heads

    // X axis (Red) - lighter color if selected
    if (appState.axisSelected[0]) {
        glColor3f(1.0f, 0.5f, 0.5f); // Light red when selected
    } else {
        glColor3f(1.0f, 0.0f, 0.0f); // Normal red
    }
    float xCenter = (xBtnX1 + xBtnX2) * 0.5f;
    glBegin(GL_LINES);
    glVertex2f(xCenter, arrowY);
    glVertex2f(xCenter + axisLength, arrowY);
    glEnd();

    // X arrow
    glBegin(GL_TRIANGLES);
    glVertex2f(xCenter + axisLength, arrowY);
    glVertex2f(xCenter + axisLength - arrowSize, arrowY - arrowSize);
    glVertex2f(xCenter + axisLength - arrowSize, arrowY + arrowSize);
    glEnd();

    // Y axis (Green) - lighter color if selected
    if (appState.axisSelected[1]) {
        glColor3f(0.5f, 1.0f, 0.5f); // Light green when selected
    } else {
        glColor3f(0.0f, 1.0f, 0.0f); // Normal green
    }
    float yCenter = (yBtnX1 + yBtnX2) * 0.5f;
    glBegin(GL_LINES);
    glVertex2f(yCenter, arrowY);
    glVertex2f(yCenter, arrowY + axisLength);
    glEnd();

    // Y arrow
    glBegin(GL_TRIANGLES);
    glVertex2f(yCenter, arrowY + axisLength);
    glVertex2f(yCenter - arrowSize, arrowY + axisLength - arrowSize);
    glVertex2f(yCenter + arrowSize, arrowY + axisLength - arrowSize);
    glEnd();

    // Z axis (Blue) - lighter color if selected
    if (appState.axisSelected[2]) {
        glColor3f(0.5f, 0.5f, 1.0f); // Light blue when selected
    } else {
        glColor3f(0.0f, 0.0f, 1.0f); // Normal blue
    }
    float zCenter = (zBtnX1 + zBtnX2) * 0.5f;
    glBegin(GL_LINES);
    glVertex2f(zCenter, arrowY);
    glVertex2f(zCenter - axisLength * 0.7f, arrowY - axisLength * 0.7f);
    glEnd();

    // Z arrow
    glBegin(GL_TRIANGLES);
    glVertex2f(zCenter - axisLength * 0.7f, arrowY - axisLength * 0.7f);
    glVertex2f(zCenter - axisLength * 0.7f + arrowSize, arrowY - axisLength * 0.7f + arrowSize);
    glVertex2f(zCenter - axisLength * 0.7f + arrowSize * 0.5f, arrowY - axisLength * 0.7f - arrowSize * 0.5f);
    glEnd();

    // Draw axis labels with corresponding colors
    if (appState.axisSelected[0]) {
        glColor3f(1.0f, 0.5f, 0.5f);
    } else {
        glColor3f(1.0f, 0.0f, 0.0f);
    }
    PrimitiveRenderer::drawText("X", xCenter + axisLength + 0.003f, arrowY - 0.002f, GLUT_BITMAP_HELVETICA_10);

    if (appState.axisSelected[1]) {
        glColor3f(0.5f, 1.0f, 0.5f);
    } else {
        glColor3f(0.0f, 1.0f, 0.0f);
    }
    PrimitiveRenderer::drawText("Y", yCenter - 0.002f, arrowY + axisLength + 0.003f, GLUT_BITMAP_HELVETICA_10);

    if (appState.axisSelected[2]) {
        glColor3f(0.5f, 0.5f, 1.0f);
    } else {
        glColor3f(0.0f, 0.0f, 1.0f);
    }
    PrimitiveRenderer::drawText("Z", zCenter - axisLength * 0.7f - 0.005f, arrowY - axisLength * 0.7f - 0.005f, GLUT_BITMAP_HELVETICA_10);

    // Restore matrix state
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// ... rest of the file remains exactly the same until the main function ...

void checkShapeButtonClicks(double xpos, double ypos, int width, int height, ApplicationState& appState) {
    // Calculate the shapes panel position
    float leftPanelWidth = PrimitiveRenderer::pxToNDCx((int)(Constants::LEFT_BAR_WIDTH * 0.85f), width);
    float leftPanelHeight = PrimitiveRenderer::pxToNDCy(310, height);
    float leftPanelGap = PrimitiveRenderer::pxToNDCy(20, height);
    float leftVerticalOffset = PrimitiveRenderer::pxToNDCy(5, height);

    float texturesPanelY = -1.0f + appState.leftPanelTexturesScroll + leftVerticalOffset;
    float shapesPanelY = texturesPanelY + leftPanelHeight + leftPanelGap + appState.leftPanelShapesScroll;

    float panelX1 = -1.0f + PrimitiveRenderer::pxToNDCx(5, width);
    float buttonWidth = (leftPanelWidth - PrimitiveRenderer::pxToNDCx(40, width)) / 2.0f;
    float buttonHeight = PrimitiveRenderer::pxToNDCy(75, height);
    float buttonGap = PrimitiveRenderer::pxToNDCx(8, width);
    float rowGap = PrimitiveRenderer::pxToNDCy(10, height);

    float contentHeight = (buttonHeight * 3) + (rowGap * 2);
    float maxScroll = std::max(0.0f, contentHeight - leftPanelHeight);
    float scrollOffset = std::max(0.0f, std::min(maxScroll, -appState.leftPanelShapesScroll));

    float row1Y = shapesPanelY + leftPanelHeight - PrimitiveRenderer::pxToNDCy(50, height) + scrollOffset;

    // Check Cube button
    float cubeX1 = panelX1 + PrimitiveRenderer::pxToNDCx(10, width);
    float cubeX2 = cubeX1 + buttonWidth;
    if (PrimitiveRenderer::isInsideNDC(xpos, ypos, width, height, cubeX1, row1Y - buttonHeight, cubeX2, row1Y)) {
        appState.currentShape = ShapeType::CUBE;
        std::cout << "Cube generated!\n";
        return;
    }

    // Check Sphere button
    float sphereX1 = cubeX2 + buttonGap;
    float sphereX2 = sphereX1 + buttonWidth;
    if (PrimitiveRenderer::isInsideNDC(xpos, ypos, width, height, sphereX1, row1Y - buttonHeight, sphereX2, row1Y)) {
        appState.currentShape = ShapeType::SPHERE;
        std::cout << "Sphere generated!\n";
        return;
    }

    float row2Y = row1Y - buttonHeight - rowGap;

    // Check Cone button
    float coneX1 = panelX1 + PrimitiveRenderer::pxToNDCx(10, width);
    float coneX2 = coneX1 + buttonWidth;
    if (PrimitiveRenderer::isInsideNDC(xpos, ypos, width, height, coneX1, row2Y - buttonHeight, coneX2, row2Y)) {
        appState.currentShape = ShapeType::CONE;
        std::cout << "Cone generated!\n";
        return;
    }

    // Check Cylinder button
    float cylinderX1 = coneX2 + buttonGap;
    float cylinderX2 = cylinderX1 + buttonWidth;
    if (PrimitiveRenderer::isInsideNDC(xpos, ypos, width, height, cylinderX1, row2Y - buttonHeight, cylinderX2, row2Y)) {
        appState.currentShape = ShapeType::CYLINDER;
        std::cout << "Cylinder generated!\n";
        return;
    }

    float row3Y = row2Y - buttonHeight - rowGap;

    // Check Torus button
    float torusX1 = panelX1 + PrimitiveRenderer::pxToNDCx(10, width);
    float torusX2 = torusX1 + buttonWidth;
    if (PrimitiveRenderer::isInsideNDC(xpos, ypos, width, height, torusX1, row3Y - buttonHeight, torusX2, row3Y)) {
        appState.currentShape = ShapeType::TORUS;
        std::cout << "Torus generated!\n";
        return;
    }

    // Check Pyramid button
    float pyramidX1 = torusX2 + buttonGap;
    float pyramidX2 = pyramidX1 + buttonWidth;
    if (PrimitiveRenderer::isInsideNDC(xpos, ypos, width, height, pyramidX1, row3Y - buttonHeight, pyramidX2, row3Y)) {
        appState.currentShape = ShapeType::PYRAMID;
        std::cout << "Pyramid generated!\n";
        return;
    }
}

// Text input handling functions
void handleTextInput(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (!appState.activeInputField.active) return;

    if (action == GLFW_PRESS) {
        // Handle backspace
        if (key == GLFW_KEY_BACKSPACE) {
            if (!appState.activeInputField.text.empty()) {
                appState.activeInputField.text.pop_back();
            }
        }
        // Handle enter to apply the value
        else if (key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER) {
            if (!appState.activeInputField.text.empty()) {
                try {
                    float newValue = std::stof(appState.activeInputField.text);

                    // Apply the new value to the appropriate vector
                    if (appState.activeInputField.panelType == 0) { // Rotation
                        appState.rotate[appState.activeInputField.axis] = newValue;
                        std::cout << "Rotation " << (appState.activeInputField.axis == 0 ? "X" :
                        appState.activeInputField.axis == 1 ? "Y" : "Z")
                        << " set to: " << newValue << std::endl;
                    }
                    else if (appState.activeInputField.panelType == 1) { // Scaling
                        appState.scale[appState.activeInputField.axis] = newValue;
                        std::cout << "Scaling " << (appState.activeInputField.axis == 0 ? "X" :
                        appState.activeInputField.axis == 1 ? "Y" : "Z")
                        << " set to: " << newValue << std::endl;
                    }
                    else if (appState.activeInputField.panelType == 2) { // Translation
                        appState.translate[appState.activeInputField.axis] = newValue;
                        std::cout << "Translation " << (appState.activeInputField.axis == 0 ? "X" :
                        appState.activeInputField.axis == 1 ? "Y" : "Z")
                        << " set to: " << newValue << std::endl;
                    }
                }
                catch (const std::exception& e) {
                    std::cout << "Invalid input: " << e.what() << std::endl;
                }
            }
            // Clear the input field
            appState.activeInputField.active = false;
            appState.activeInputField.text.clear();
        }
        // Handle escape to cancel input
        else if (key == GLFW_KEY_ESCAPE) {
            appState.activeInputField.active = false;
            appState.activeInputField.text.clear();
            std::cout << "Input cancelled\n";
        }
    }
}

void handleCharacterInput(GLFWwindow* window, unsigned int codepoint) {
    if (!appState.activeInputField.active) return;

    // Only allow digits, decimal point, and minus sign
    if ((codepoint >= '0' && codepoint <= '9') || codepoint == '.' || codepoint == '-') {
        // Prevent multiple decimal points
        if (codepoint == '.' && appState.activeInputField.text.find('.') != std::string::npos) {
            return;
        }
        // Prevent multiple minus signs and ensure minus is only at the beginning
        if (codepoint == '-') {
            if (!appState.activeInputField.text.empty()) {
                return;
            }
        }

        appState.activeInputField.text += static_cast<char>(codepoint);
    }
}

int main() {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
    GLFWwindow* window = glfwCreateWindow(videoMode->width, videoMode->height, "Blender Life", primaryMonitor, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Add these new callbacks for text input
    glfwSetKeyCallback(window, handleTextInput);
    glfwSetCharCallback(window, handleCharacterInput);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    // Initialize textures
    PrimitiveRenderer::initTextures();

    int argc = 0;
    char** argv = nullptr;
    glutInit(&argc, argv);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.12f, 0.12f, 0.15f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // Clear both color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        // Use fixed sidebar widths from Constants
        if (width < Constants::MIN_WINDOW_WIDTH + Constants::LEFT_BAR_WIDTH + Constants::RIGHT_BAR_WIDTH) {
            width = Constants::MIN_WINDOW_WIDTH + Constants::LEFT_BAR_WIDTH + Constants::RIGHT_BAR_WIDTH;
        }

        // Calculate edges using fixed widths
        float leftEdgeNDC = -1.0f + (2.0f * Constants::LEFT_BAR_WIDTH / width);
        float rightEdgeNDC = 1.0f - (2.0f * Constants::RIGHT_BAR_WIDTH / width);
        float topEdgeNDC = 1.0f - (2.0f * Constants::TOP_BAR_HEIGHT / height);

        // Set viewport to full window
        glViewport(0, 0, width, height);

        // Set up proper 2D projection for the entire window first
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Draw canvas background first
        float canvasX1 = leftEdgeNDC + PrimitiveRenderer::pxToNDCx(6, width);
        float canvasX2 = rightEdgeNDC - PrimitiveRenderer::pxToNDCx(6, width);
        float canvasY1 = -1.0f;
        float canvasY2 = topEdgeNDC - PrimitiveRenderer::pxToNDCy(6, height);

        PrimitiveRenderer::drawRect(canvasX1, canvasY1, canvasX2, canvasY2, 0.12f, 0.12f, 0.15f);
        PrimitiveRenderer::drawOutlineRect(canvasX1, canvasY1, canvasX2, canvasY2, 0.2f, 0.2f, 0.2f, 1.0f);

        // Now draw 3D shapes with proper depth testing
        glEnable(GL_DEPTH_TEST);
        drawCurrentShape(appState);
        glDisable(GL_DEPTH_TEST);

        // Now draw UI elements on top
        // Draw static sidebars
        PrimitiveRenderer::drawRect(-1.0f, -1.0f, leftEdgeNDC, topEdgeNDC, 0.5f, 0.5f, 0.5f);
        PrimitiveRenderer::drawRect(rightEdgeNDC, -1.0f, 1.0f, topEdgeNDC, 0.5f, 0.5f, 0.5f);
        PrimitiveRenderer::drawRect(-1.0f, topEdgeNDC, 1.0f, 1.0f, 0.45f, 0.45f, 0.45f);

        // Draw UI panels
        Panels::drawTopBarUI(width, height, appState);

        float leftPanelWidth = PrimitiveRenderer::pxToNDCx((int)(Constants::LEFT_BAR_WIDTH * 0.85f), width);
        float leftPanelHeight = PrimitiveRenderer::pxToNDCy(310, height);
        float leftPanelGap = PrimitiveRenderer::pxToNDCy(20, height);
        float leftVerticalOffset = PrimitiveRenderer::pxToNDCy(5, height);

        float texturesPanelY = -1.0f + appState.leftPanelTexturesScroll + leftVerticalOffset;
        Panels::drawTexturesPanel(-1.0f + PrimitiveRenderer::pxToNDCx(5, width), texturesPanelY,
            leftPanelWidth, leftPanelHeight, appState, width, height);

        float shapesPanelY = texturesPanelY + leftPanelHeight + leftPanelGap + appState.leftPanelShapesScroll;
        Panels::drawShapesPanel(-1.0f + PrimitiveRenderer::pxToNDCx(5, width), shapesPanelY,
            leftPanelWidth, leftPanelHeight, appState, width, height);

        float panelWidth = PrimitiveRenderer::pxToNDCx((int)(Constants::RIGHT_BAR_WIDTH * 0.85f), width);
        float panelHeight = PrimitiveRenderer::pxToNDCy(180, height);
        float panelGap = PrimitiveRenderer::pxToNDCy(15, height);
        float totalPanelsHeight = (panelHeight * 3) + (panelGap * 2);
        float rightVerticalOffset = PrimitiveRenderer::pxToNDCy(50, height);

        Panels::drawTransformPanel(rightEdgeNDC + PrimitiveRenderer::pxToNDCx(5, width), -1.0f + appState.rightPanelScroll + rightVerticalOffset,
            panelWidth, panelHeight,
            "Rotation", appState.rotate, appState, width, height);

        float scalePanelY = -1.0f + panelHeight + panelGap + appState.rightPanelScroll + rightVerticalOffset;
        Panels::drawTransformPanel(rightEdgeNDC + PrimitiveRenderer::pxToNDCx(5, width), scalePanelY,
            panelWidth, panelHeight,
            "Scaling", appState.scale, appState, width, height);

        float translatePanelY = -1.0f + (panelHeight * 2) + (panelGap * 2) + appState.rightPanelScroll + rightVerticalOffset;
        Panels::drawTransformPanel(rightEdgeNDC + PrimitiveRenderer::pxToNDCx(5, width), translatePanelY,
            panelWidth, panelHeight,
            "Translate", appState.translate, appState, width, height);

        // Draw scroll bars
        float scrollBarWidth = PrimitiveRenderer::pxToNDCx(8, width);
        float scrollBarX = 1.0f - scrollBarWidth;
        float scrollBarHeight = topEdgeNDC - (-1.0f);
        float scrollThumbHeight = scrollBarHeight * 0.3f;
        float maxScroll = totalPanelsHeight - (topEdgeNDC - (-1.0f));
        if (maxScroll < 0) maxScroll = 0;
        float scrollProgress = 0.0f;
        if (maxScroll > 0) {
            scrollProgress = std::max(0.0f, std::min(1.0f, -appState.rightPanelScroll / maxScroll));
        }
        float scrollThumbY = -1.0f + (1.0f - scrollProgress) * scrollBarHeight;

        PrimitiveRenderer::drawRect(scrollBarX, -1.0f, scrollBarX + scrollBarWidth, topEdgeNDC, 0.3f, 0.3f, 0.3f);
        PrimitiveRenderer::drawRect(scrollBarX, scrollThumbY - scrollThumbHeight, scrollBarX + scrollBarWidth, scrollThumbY, 0.6f, 0.6f, 0.6f);

        // REMOVED: draw3DAxes call - axes are now drawn above the buttons
        // Draw axis buttons with vector arrows above them
        drawAxisButtons(leftEdgeNDC, canvasY1, canvasY2, width, height);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    // Cleanup textures
    PrimitiveRenderer::cleanupTextures();

    return 0;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;
    glViewport(0,0,width,height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        appState.mouseClicked = true; // Set mouse clicked flag

        // Store the original values before deactivating (for cancellation)
        static float originalValues[3] = {0.0f, 0.0f, 0.0f};
        static bool hasOriginalValues = false;

        // If we're currently editing and clicking outside, restore original values
        if (appState.activeInputField.active) {
            // Restore original values if text is empty (user didn't enter anything)
            if (appState.activeInputField.text.empty() && hasOriginalValues) {
                if (appState.activeInputField.panelType == 0) { // Rotation
                    appState.rotate[0] = originalValues[0];
                    appState.rotate[1] = originalValues[1];
                    appState.rotate[2] = originalValues[2];
                } else if (appState.activeInputField.panelType == 1) { // Scaling
                    appState.scale[0] = originalValues[0];
                    appState.scale[1] = originalValues[1];
                    appState.scale[2] = originalValues[2];
                } else if (appState.activeInputField.panelType == 2) { // Translation
                    appState.translate[0] = originalValues[0];
                    appState.translate[1] = originalValues[1];
                    appState.translate[2] = originalValues[2];
                }
                std::cout << "Input cancelled - restored original values\n";
            }

            appState.activeInputField.active = false;
            appState.activeInputField.text.clear();
            hasOriginalValues = false;
            std::cout << "Input field deactivated\n";
        }

        // Check shape button clicks first
        checkShapeButtonClicks(xpos, ypos, width, height, appState);

        // Toggle axis selection when clicking axis buttons
        if (appState.axisHovered[0]) {
            appState.axisSelected[0] = !appState.axisSelected[0];
            std::cout << "X axis " << (appState.axisSelected[0] ? "selected" : "deselected") << "!\n";
        }
        if (appState.axisHovered[1]) {
            appState.axisSelected[1] = !appState.axisSelected[1];
            std::cout << "Y axis " << (appState.axisSelected[1] ? "selected" : "deselected") << "!\n";
        }
        if (appState.axisHovered[2]) {
            appState.axisSelected[2] = !appState.axisSelected[2];
            std::cout << "Z axis " << (appState.axisSelected[2] ? "selected" : "deselected") << "!\n";
        }

        // Check if we're starting to drag any selected axis
        bool anyAxisSelected = appState.axisSelected[0] || appState.axisSelected[1] || appState.axisSelected[2];
        if (anyAxisSelected) {
            appState.draggingAxis = true;
            appState.dragStartX = xpos;
            appState.dragStartY = ypos;
            std::cout << "Started dragging selected axes\n";
        }

        // Print current selection state
        std::cout << "Current selection - X:" << appState.axisSelected[0]
        << " Y:" << appState.axisSelected[1]
        << " Z:" << appState.axisSelected[2] << "\n";

    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        appState.draggingAxis = false;
        appState.mouseClicked = false; // Reset mouse clicked flag
        std::cout << "Stopped dragging\n";
    }
    (void)mods;
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    appState.mouseX = xpos;
    appState.mouseY = ypos;

    // Handle axis dragging
    if (appState.draggingAxis) {
        double dx = xpos - appState.dragStartX;
        double dy = ypos - appState.dragStartY;

        // Update translation based on selected axes
        if (appState.axisSelected[0]) { // X axis
            appState.translate[0] += dx * 0.01f; // Scale factor for sensitivity
        }
        if (appState.axisSelected[1]) { // Y axis
            appState.translate[1] -= dy * 0.01f; // Invert Y for natural dragging
        }
        if (appState.axisSelected[2]) { // Z axis
            appState.translate[2] += (dx + dy) * 0.005f; // Combine X and Y for Z movement
        }

        appState.dragStartX = xpos;
        appState.dragStartY = ypos;

        std::cout << "Translation - X:" << appState.translate[0]
        << " Y:" << appState.translate[1]
        << " Z:" << appState.translate[2] << "\n";
        return; // Skip other logic when dragging axes
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)window;
    (void)xoffset;

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    double mx, my;
    glfwGetCursorPos(window, &mx, &my);

    if (mx < Constants::LEFT_BAR_WIDTH) {
        if (my < height / 2) {
            appState.leftPanelShapesScroll += yoffset * 20.0f;
            float maxShapesScroll = 400.0f;
            appState.leftPanelShapesScroll = std::max(-maxShapesScroll, std::min(0.0f, appState.leftPanelShapesScroll));
        } else {
            appState.leftPanelTexturesScroll += yoffset * 20.0f;
            float maxTexturesScroll = 400.0f;
            appState.leftPanelTexturesScroll = std::max(-maxTexturesScroll, std::min(0.0f, appState.leftPanelTexturesScroll));
        }
    } else {
        appState.rightPanelScroll += yoffset * 20.0f;
        float maxScroll = 400.0f;
        appState.rightPanelScroll = std::max(-maxScroll, std::min(0.0f, appState.rightPanelScroll));
    }
}