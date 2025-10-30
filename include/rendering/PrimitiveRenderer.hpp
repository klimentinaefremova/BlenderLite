#ifndef PRIMITIVE_RENDERER_HPP
#define PRIMITIVE_RENDERER_HPP

#include <GL/glut.h>
#include <string>
#include <vector>
#include "core/ApplicationState.hpp"

class PrimitiveRenderer {
public:
    static void drawRect(float x1, float y1, float x2, float y2, float r, float g, float b);
    static void drawOutlineRect(float x1, float y1, float x2, float y2, float lineR, float lineG, float lineB, float lineWidth = 2.0f);
    static void drawCircle(float cx, float cy, float radius, int segments, float r, float g, float b);
    static void drawText(const std::string &text, float x, float y, void* font = GLUT_BITMAP_HELVETICA_12);
    static float pxToNDCx(int px, int width);
    static float pxToNDCy(int px, int height);
    static void mouseToNDC(double mx, double my, int width, int height, float &ndcX, float &ndcY);
    static bool isInsideNDC(double mx, double my, int width, int height, float x1, float y1, float x2, float y2);
    static void setScissor(int x, int y, int width, int height, int screenHeight);

    // New texture-related functions
    static bool loadTexture(const std::string& filename, GLuint& textureID);
    static void drawTexturedRect(float x1, float y1, float x2, float y2, GLuint textureID);
    static void initTextures();
    static void cleanupTextures();

    // NEW: Texture application functions
    static void applyTextureToShape(ShapeType shapeType, int textureID, ApplicationState& appState);
    static void applyColorToShape(ShapeType shapeType, ApplicationState& appState);
    static int getShapeTexture(ShapeType shapeType, ApplicationState& appState);
    static bool shapeHasTexture(ShapeType shapeType, ApplicationState& appState);
    static void drawTexturedShape(ShapeType shapeType, ApplicationState& appState);

    // Texture storage
    static std::vector<GLuint> textureIDs;
};

#endif