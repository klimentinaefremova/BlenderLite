#include "../include/rendering/PrimitiveRenderer.hpp"
#include "../include/core/Constants.hpp"
#include <cmath>

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