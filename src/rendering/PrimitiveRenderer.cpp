#include "../include/rendering/PrimitiveRenderer.hpp"
#include "../include/core/Constants.hpp"
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Initialize static member
std::vector<GLuint> PrimitiveRenderer::textureIDs;

// NEW: Function to apply texture to a shape
void PrimitiveRenderer::applyTextureToShape(ShapeType shapeType, int textureID, ApplicationState& appState) {
    if (shapeType != ShapeType::NONE) {
        int shapeIndex = static_cast<int>(shapeType) - 1; // Convert enum to index (0-5)
        if (shapeIndex >= 0 && shapeIndex < 6) {
            appState.shapeTextures[shapeIndex] = textureID;
            appState.shapeUsesTexture[shapeIndex] = true; // Mark as using texture
            std::cout << "Applied texture " << textureID << " to shape " << shapeIndex << std::endl;
        }
    }
}

// NEW: Function to apply color to a shape (remove texture)
void PrimitiveRenderer::applyColorToShape(ShapeType shapeType, ApplicationState& appState) {
    if (shapeType != ShapeType::NONE) {
        int shapeIndex = static_cast<int>(shapeType) - 1;
        if (shapeIndex >= 0 && shapeIndex < 6) {
            appState.shapeUsesTexture[shapeIndex] = false; // Mark as using color
            std::cout << "Applied color to shape " << shapeIndex << std::endl;
        }
    }
}

// NEW: Function to get texture for a shape
int PrimitiveRenderer::getShapeTexture(ShapeType shapeType, ApplicationState& appState) {
    if (shapeType != ShapeType::NONE) {
        int shapeIndex = static_cast<int>(shapeType) - 1;
        if (shapeIndex >= 0 && shapeIndex < 6) {
            return appState.shapeTextures[shapeIndex];
        }
    }
    return -1;
}

// NEW: Function to check if shape uses texture
bool PrimitiveRenderer::shapeHasTexture(ShapeType shapeType, ApplicationState& appState) {
    if (shapeType != ShapeType::NONE) {
        int shapeIndex = static_cast<int>(shapeType) - 1;
        if (shapeIndex >= 0 && shapeIndex < 6) {
            return appState.shapeUsesTexture[shapeIndex];
        }
    }
    return false;
}

void PrimitiveRenderer::drawRect(float x1, float y1, float x2, float y2, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

void PrimitiveRenderer::drawOutlineRect(float x1, float y1, float x2, float y2, float lineR, float lineG, float lineB, float lineWidth) {
    glColor3f(lineR, lineG, lineB);
    glLineWidth(lineWidth);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

void PrimitiveRenderer::drawCircle(float cx, float cy, float radius, int segments, float r, float g, float b) {
    glColor3f(r,g,b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i=0;i<=segments;i++){
        float a = (float)i / (float)segments * 2.0f * Constants::PI;
        glVertex2f(cx + cosf(a)*radius, cy + sinf(a)*radius);
    }
    glEnd();
}

void PrimitiveRenderer::drawText(const std::string &text, float x, float y, void* font) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(font, (int)c);
    }
}

float PrimitiveRenderer::pxToNDCx(int px, int width) {
    return 2.0f * px / (float)width;
}

float PrimitiveRenderer::pxToNDCy(int px, int height) {
    return 2.0f * px / (float)height;
}

void PrimitiveRenderer::mouseToNDC(double mx, double my, int width, int height, float &ndcX, float &ndcY) {
    ndcX = (float)((mx / (double)width) * 2.0 - 1.0);
    ndcY = (float)(1.0 - (my / (double)height) * 2.0);
}

bool PrimitiveRenderer::isInsideNDC(double mx, double my, int width, int height, float x1, float y1, float x2, float y2) {
    float nx, ny;
    mouseToNDC(mx, my, width, height, nx, ny);
    return nx >= x1 && nx <= x2 && ny >= y1 && ny <= y2;
}

void PrimitiveRenderer::setScissor(int x, int y, int width, int height, int screenHeight) {
    glScissor(x, screenHeight - y - height, width, height);
}

bool PrimitiveRenderer::loadTexture(const std::string& filename, GLuint& textureID) {
    int width, height, nrChannels;

    std::cout << "DEBUG: Attempting to load texture: " << filename << std::endl;

    // Check if file exists
    std::ifstream file(filename);
    if (!file.good()) {
        std::cout << "ERROR: File does not exist or cannot be opened: " << filename << std::endl;
        return false;
    }
    file.close();

    // Don't flip vertically for OpenGL texture coordinates
    stbi_set_flip_vertically_on_load(false);

    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        std::cout << "Loading texture: " << filename << " (" << width << "x" << height << ", channels: " << nrChannels << ")" << std::endl;

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set texture parameters - USE COMPATIBLE CONSTANTS
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum format = GL_RGB;
        if (nrChannels == 4) {
            format = GL_RGBA;
        } else if (nrChannels == 3) {
            format = GL_RGB;
        } else if (nrChannels == 1) {
            format = GL_LUMINANCE;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        // Generate mipmaps
        gluBuild2DMipmaps(GL_TEXTURE_2D, format, width, height, format, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);

        // Check for OpenGL errors
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cout << "OpenGL error after loading texture: " << error << std::endl;
            return false;
        }

        std::cout << "Successfully loaded texture ID: " << textureID << std::endl;
        return true;
    } else {
        std::cout << "Failed to load texture: " << filename << " - " << stbi_failure_reason() << std::endl;
        return false;
    }
}

void PrimitiveRenderer::drawTexturedRect(float x1, float y1, float x2, float y2, GLuint textureID) {
    if (textureID == 0) {
        // Draw a colored fallback if texture is invalid
        PrimitiveRenderer::drawRect(x1, y1, x2, y2, 0.8f, 0.2f, 0.2f);
        return;
    }

    // Save current state
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Enable blending for textures with transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set color to white to preserve texture colors
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    // Correct texture coordinates - match vertex order
    glTexCoord2f(0.0f, 1.0f); glVertex2f(x1, y1); // Bottom-left
    glTexCoord2f(1.0f, 1.0f); glVertex2f(x2, y1); // Bottom-right
    glTexCoord2f(1.0f, 0.0f); glVertex2f(x2, y2); // Top-right
    glTexCoord2f(0.0f, 0.0f); glVertex2f(x1, y2); // Top-left
    glEnd();

    // Restore state
    glPopAttrib();
}

void PrimitiveRenderer::initTextures() {
    std::cout << "Initializing textures..." << std::endl;

    // Load all textures as PNG files
    std::vector<std::string> textureFiles = {
        "textures/texture1.png",
        "textures/texture2.png",
        "textures/texture3.png",
        "textures/texture4.png",
        "textures/texture5.png",
        "textures/texture6.png"
    };

    textureIDs.resize(textureFiles.size(), 0);
    int loadedCount = 0;

    // Enable texturing globally
    glEnable(GL_TEXTURE_2D);

    for (size_t i = 0; i < textureFiles.size(); ++i) {
        GLuint textureID = 0;
        if (loadTexture(textureFiles[i], textureID)) {
            textureIDs[i] = textureID;
            loadedCount++;
            std::cout << "Assigned texture " << textureFiles[i] << " to slot " << i << " with ID " << textureID << std::endl;
        } else {
            std::cout << "Creating fallback texture for slot " << i << std::endl;
            // Create a fallback texture that shows the texture number
            glGenTextures(1, &textureIDs[i]);
            glBindTexture(GL_TEXTURE_2D, textureIDs[i]);

            // Create a simple colored texture with the texture number
            const int texSize = 64;
            unsigned char fallbackData[texSize * texSize * 3];

            for (int y = 0; y < texSize; y++) {
                for (int x = 0; x < texSize; x++) {
                    int index = (y * texSize + x) * 3;

                    // Create a colored background based on texture index
                    float hue = (i * 60.0f) / 360.0f; // Different color for each texture
                    float r, g, b;

                    // Simple HSV to RGB conversion
                    int hi = (int)(hue * 6);
                    float f = hue * 6 - hi;
                    float p = 0.0f;
                    float q = 1.0f - f;
                    float t = f;

                    switch (hi) {
                        case 0: r = 1.0f; g = t; b = p; break;
                        case 1: r = q; g = 1.0f; b = p; break;
                        case 2: r = p; g = 1.0f; b = t; break;
                        case 3: r = p; g = q; b = 1.0f; break;
                        case 4: r = t; g = p; b = 1.0f; break;
                        default: r = 1.0f; g = p; b = q; break;
                    }

                    // Add a pattern to show it's a fallback
                    bool isBorder = (x < 4 || x >= texSize-4 || y < 4 || y >= texSize-4);
                    bool isCenter = (x >= texSize/2-8 && x < texSize/2+8 && y >= texSize/2-8 && y < texSize/2+8);

                    if (isBorder) {
                        fallbackData[index] = 255;
                        fallbackData[index + 1] = 255;
                        fallbackData[index + 2] = 255;
                    } else if (isCenter) {
                        // Draw the texture number in the center
                        fallbackData[index] = (unsigned char)(r * 255);
                        fallbackData[index + 1] = (unsigned char)(g * 255);
                        fallbackData[index + 2] = (unsigned char)(b * 255);
                    } else {
                        fallbackData[index] = (unsigned char)(r * 128);
                        fallbackData[index + 1] = (unsigned char)(g * 128);
                        fallbackData[index + 2] = (unsigned char)(b * 128);
                    }
                }
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texSize, texSize, 0, GL_RGB, GL_UNSIGNED_BYTE, fallbackData);

            // Generate mipmaps
            gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, texSize, texSize, GL_RGB, GL_UNSIGNED_BYTE, fallbackData);

            std::cout << "Created fallback texture with ID: " << textureIDs[i] << std::endl;
        }
    }

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

    std::cout << "Texture initialization complete. Loaded " << loadedCount << " out of " << textureFiles.size() << " textures." << std::endl;
    std::cout << "Texture IDs: ";
    for (size_t i = 0; i < textureIDs.size(); ++i) {
        std::cout << textureIDs[i] << " ";
    }
    std::cout << std::endl;
}

void PrimitiveRenderer::cleanupTextures() {
    std::cout << "Cleaning up textures..." << std::endl;
    for (GLuint textureID : textureIDs) {
        if (textureID != 0) {
            glDeleteTextures(1, &textureID);
            std::cout << "Deleted texture ID: " << textureID << std::endl;
        }
    }
    textureIDs.clear();
}