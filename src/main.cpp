#include <glad/glad.h>
#include <GLFW/glfw3.h>

// prevent freeglut from trying to redefine main / mess with atexit on Windows
#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/glut.h>

#include <iostream>
#include <cmath>
#include <algorithm>
#include <string>

const double M_PI_DOUBLE = 3.14159265358979323846;
const float M_PI = (float)M_PI_DOUBLE; // keep float M_PI usage

// Window settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 600;
const float MIN_LEFT_BAR_WIDTH = 180.0f; // Increased minimum width
const float MIN_RIGHT_BAR_WIDTH = 280.0f; // Increased for vector panels
const float MIN_WINDOW_WIDTH = 650.0f; // below this, stop resizing behavior

// Bar variables (pixel units)
float leftBarWidth = 220.0f; // Increased initial width
float rightBarWidth = 280.0f; // Increased for vector panels
const float topBarHeight = 110.0f; // make static (const) to avoid top resizing

// Handle size (pixels)
const float HANDLE_PX = 10.0f;

// State
bool resizingLeft = false;
bool resizingRight = false;
double lastX = 0.0, lastY = 0.0;
double mouseX = 0.0, mouseY = 0.0;

// Transformation values
float translate[3] = {0.0f, 0.0f, 0.0f};
float scale[3] = {1.0f, 1.0f, 1.0f};
float rotate[3] = {0.0f, 0.0f, 0.0f};

// Scroll offset for right panel
float rightPanelScroll = 0.0f;
// Scroll offset for left panel shapes
float leftPanelShapesScroll = 0.0f;
// Scroll offset for left panel textures
float leftPanelTexturesScroll = 0.0f;

// Helpers
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void drawRect(float x1, float y1, float x2, float y2, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

void drawOutlineRect(float x1, float y1, float x2, float y2, float lineR, float lineG, float lineB, float lineWidth = 2.0f) {
    glColor3f(lineR, lineG, lineB);
    glLineWidth(lineWidth);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

// draw filled circle (for camera lens)
void drawCircle(float cx, float cy, float radius, int segments, float r, float g, float b) {
    glColor3f(r,g,b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i=0;i<=segments;i++){
        float a = (float)i / (float)segments * 2.0f * M_PI;
        glVertex2f(cx + cosf(a)*radius, cy + sinf(a)*radius);
    }
    glEnd();
}

// text helper using GLUT bitmap fonts
void drawText(const std::string &text, float x, float y, void* font = GLUT_BITMAP_HELVETICA_12) {
    // x,y are NDC coords
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(font, (int)c);
    }
}

// convert pixel to NDC delta
inline float pxToNDCx(int px, int width) { return 2.0f * px / (float)width; }
inline float pxToNDCy(int px, int height) { return 2.0f * px / (float)height; }

// Convert mouse pixel coords to NDC
void mouseToNDC(double mx, double my, int width, int height, float &ndcX, float &ndcY) {
    ndcX = (float)((mx / (double)width) * 2.0 - 1.0);
    ndcY = (float)(1.0 - (my / (double)height) * 2.0);
}

// hit test in NDC
bool isInsideNDC(double mx, double my, int width, int height, float x1, float y1, float x2, float y2) {
    float nx, ny;
    mouseToNDC(mx, my, width, height, nx, ny);
    return nx >= x1 && nx <= x2 && ny >= y1 && ny <= y2;
}

// Set scissor region in pixels
void setScissor(int x, int y, int width, int height, int screenHeight) {
    glScissor(x, screenHeight - y - height, width, height);
}

// Draw transformation panel
void drawTransformPanel(float panelX1, float panelY1, float panelWidth, float panelHeight,
                       const std::string& title, float values[3], int width, int height) {
    // Panel background
    drawRect(panelX1, panelY1, panelX1 + panelWidth, panelY1 + panelHeight, 0.4f, 0.4f, 0.4f);
    drawOutlineRect(panelX1, panelY1, panelX1 + panelWidth, panelY1 + panelHeight, 0.0f, 0.0f, 0.0f, 2.0f);

    // Title
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(title, panelX1 + pxToNDCx(10, width), panelY1 + panelHeight - pxToNDCy(25, height), GLUT_BITMAP_HELVETICA_18);

    // "Vector" label
    drawText("Vector", panelX1 + pxToNDCx(15, width), panelY1 + panelHeight - pxToNDCy(45, height), GLUT_BITMAP_HELVETICA_12);

    float inputBoxHeight = pxToNDCy(25, height);
    float inputBoxWidth = panelWidth - pxToNDCx(30, width);
    float inputY = panelY1 + panelHeight - pxToNDCy(70, height);

    // X input box
    drawRect(panelX1 + pxToNDCx(15, width), inputY - inputBoxHeight,
             panelX1 + pxToNDCx(15, width) + inputBoxWidth, inputY, 0.9f, 0.9f, 0.9f);
    drawOutlineRect(panelX1 + pxToNDCx(15, width), inputY - inputBoxHeight,
                   panelX1 + pxToNDCx(15, width) + inputBoxWidth, inputY, 0.0f, 0.0f, 0.0f, 1.0f);

    // X label and value
    glColor3f(0.2f, 0.2f, 0.2f);
    char xValue[20];
    snprintf(xValue, sizeof(xValue), "X: %7.3f", values[0]);
    drawText(xValue, panelX1 + pxToNDCx(20, width), inputY - pxToNDCy(15, height), GLUT_BITMAP_HELVETICA_12);

    // Y input box
    inputY -= inputBoxHeight + pxToNDCy(2, height);
    drawRect(panelX1 + pxToNDCx(15, width), inputY - inputBoxHeight,
             panelX1 + pxToNDCx(15, width) + inputBoxWidth, inputY, 0.9f, 0.9f, 0.9f);
    drawOutlineRect(panelX1 + pxToNDCx(15, width), inputY - inputBoxHeight,
                   panelX1 + pxToNDCx(15, width) + inputBoxWidth, inputY, 0.0f, 0.0f, 0.0f, 1.0f);

    // Y label and value
    char yValue[20];
    snprintf(yValue, sizeof(yValue), "Y: %7.3f", values[1]);
    drawText(yValue, panelX1 + pxToNDCx(20, width), inputY - pxToNDCy(15, height), GLUT_BITMAP_HELVETICA_12);

    // Z input box
    inputY -= inputBoxHeight + pxToNDCy(2, height);
    drawRect(panelX1 + pxToNDCx(15, width), inputY - inputBoxHeight,
             panelX1 + pxToNDCx(15, width) + inputBoxWidth, inputY, 0.9f, 0.9f, 0.9f);
    drawOutlineRect(panelX1 + pxToNDCx(15, width), inputY - inputBoxHeight,
                   panelX1 + pxToNDCx(15, width) + inputBoxWidth, inputY, 0.0f, 0.0f, 0.0f, 1.0f);

    // Z label and value
    char zValue[20];
    snprintf(zValue, sizeof(zValue), "Z: %7.3f", values[2]);
    drawText(zValue, panelX1 + pxToNDCx(20, width), inputY - pxToNDCy(15, height), GLUT_BITMAP_HELVETICA_12);

    // Reset button
    float resetBtnY = inputY - inputBoxHeight - pxToNDCy(10, height);
    float resetBtnWidth = pxToNDCx(60, width);
    float resetBtnHeight = pxToNDCy(25, height);
    bool hoverReset = isInsideNDC(mouseX, mouseY, width, height,
                                 panelX1 + pxToNDCx(15, width), resetBtnY - resetBtnHeight,
                                 panelX1 + pxToNDCx(15, width) + resetBtnWidth, resetBtnY);

    drawRect(panelX1 + pxToNDCx(15, width), resetBtnY - resetBtnHeight,
             panelX1 + pxToNDCx(15, width) + resetBtnWidth, resetBtnY,
             0.32f + (hoverReset ? 0.08f : 0.0f), 0.32f + (hoverReset ? 0.08f : 0.0f), 0.32f + (hoverReset ? 0.08f : 0.0f));
    drawOutlineRect(panelX1 + pxToNDCx(15, width), resetBtnY - resetBtnHeight,
                   panelX1 + pxToNDCx(15, width) + resetBtnWidth, resetBtnY, 0,0,0,1.0f);

    glColor3f(1,1,1);
    drawText("Reset", panelX1 + pxToNDCx(25, width), resetBtnY - pxToNDCy(15, height), GLUT_BITMAP_HELVETICA_12);
}

// Draw Shapes Panel with scissor test
void drawShapesPanel(float panelX1, float panelY1, float panelWidth, float panelHeight, int width, int height) {
    // Convert NDC coordinates to pixel coordinates for scissor
    int panelXPx = (int)((panelX1 + 1.0f) * 0.5f * width);
    int panelYPx = (int)((1.0f - (panelY1 + panelHeight)) * 0.5f * height);
    int panelWidthPx = (int)(panelWidth * 0.5f * width);
    int panelHeightPx = (int)(panelHeight * 0.5f * height);

    // Enable scissor test for this panel
    glEnable(GL_SCISSOR_TEST);
    setScissor(panelXPx, panelYPx, panelWidthPx, panelHeightPx, height);

    // Panel background
    drawRect(panelX1, panelY1, panelX1 + panelWidth, panelY1 + panelHeight, 0.4f, 0.4f, 0.4f);
    drawOutlineRect(panelX1, panelY1, panelX1 + panelWidth, panelY1 + panelHeight, 0.0f, 0.0f, 0.0f, 2.0f);

    // Title
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText("Shapes", panelX1 + pxToNDCx(10, width), panelY1 + panelHeight - pxToNDCy(25, height), GLUT_BITMAP_HELVETICA_18);

    // Shape button dimensions - properly sized to fit within panel
    float buttonWidth = (panelWidth - pxToNDCx(40, width)) / 2.0f; // Reduced for scroll bar
    float buttonHeight = pxToNDCy(75, height); // Adjusted height
    float buttonGap = pxToNDCx(8, width); // Gap between buttons

    // Calculate content height (3 rows of buttons with gaps)
    float rowGap = pxToNDCy(10, height);
    float contentHeight = (buttonHeight * 3) + (rowGap * 2);
    float maxScroll = std::max(0.0f, contentHeight - panelHeight);

    // Apply scroll offset
    float scrollOffset = std::max(0.0f, std::min(maxScroll, -leftPanelShapesScroll));

    // Row 1: Square and Sphere
    float row1Y = panelY1 + panelHeight - pxToNDCy(50, height) + scrollOffset;

    // Square button
    float squareX1 = panelX1 + pxToNDCx(10, width);
    float squareX2 = squareX1 + buttonWidth;
    bool hoverSquare = isInsideNDC(mouseX, mouseY, width, height, squareX1, row1Y - buttonHeight, squareX2, row1Y);
    drawRect(squareX1, row1Y - buttonHeight, squareX2, row1Y, 0.3f + (hoverSquare ? 0.1f : 0.0f), 0.3f + (hoverSquare ? 0.1f : 0.0f), 0.3f + (hoverSquare ? 0.1f : 0.0f));
    drawOutlineRect(squareX1, row1Y - buttonHeight, squareX2, row1Y, 0.0f, 0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText("Square", squareX1 + pxToNDCx(20, width), row1Y - pxToNDCy(35, height), GLUT_BITMAP_HELVETICA_12);
    drawText("[generate", squareX1 + pxToNDCx(15, width), row1Y - pxToNDCy(55, height), GLUT_BITMAP_HELVETICA_12);
    drawText("square]", squareX1 + pxToNDCx(20, width), row1Y - pxToNDCy(70, height), GLUT_BITMAP_HELVETICA_12);

    // Sphere button
    float sphereX1 = squareX2 + buttonGap;
    float sphereX2 = sphereX1 + buttonWidth;
    bool hoverSphere = isInsideNDC(mouseX, mouseY, width, height, sphereX1, row1Y - buttonHeight, sphereX2, row1Y);
    drawRect(sphereX1, row1Y - buttonHeight, sphereX2, row1Y, 0.3f + (hoverSphere ? 0.1f : 0.0f), 0.3f + (hoverSphere ? 0.1f : 0.0f), 0.3f + (hoverSphere ? 0.1f : 0.0f));
    drawOutlineRect(sphereX1, row1Y - buttonHeight, sphereX2, row1Y, 0.0f, 0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText("Sphere", sphereX1 + pxToNDCx(20, width), row1Y - pxToNDCy(35, height), GLUT_BITMAP_HELVETICA_12);
    drawText("[generate", sphereX1 + pxToNDCx(15, width), row1Y - pxToNDCy(55, height), GLUT_BITMAP_HELVETICA_12);
    drawText("sphere]", sphereX1 + pxToNDCx(20, width), row1Y - pxToNDCy(70, height), GLUT_BITMAP_HELVETICA_12);

    // Row 2: Cone and Cylinder
    float row2Y = row1Y - buttonHeight - rowGap;

    // Cone button
    float coneX1 = panelX1 + pxToNDCx(10, width);
    float coneX2 = coneX1 + buttonWidth;
    bool hoverCone = isInsideNDC(mouseX, mouseY, width, height, coneX1, row2Y - buttonHeight, coneX2, row2Y);
    drawRect(coneX1, row2Y - buttonHeight, coneX2, row2Y, 0.3f + (hoverCone ? 0.1f : 0.0f), 0.3f + (hoverCone ? 0.1f : 0.0f), 0.3f + (hoverCone ? 0.1f : 0.0f));
    drawOutlineRect(coneX1, row2Y - buttonHeight, coneX2, row2Y, 0.0f, 0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText("Cone", coneX1 + pxToNDCx(25, width), row2Y - pxToNDCy(35, height), GLUT_BITMAP_HELVETICA_12);
    drawText("[generate", coneX1 + pxToNDCx(15, width), row2Y - pxToNDCy(55, height), GLUT_BITMAP_HELVETICA_12);
    drawText("cone]", coneX1 + pxToNDCx(25, width), row2Y - pxToNDCy(70, height), GLUT_BITMAP_HELVETICA_12);

    // Cylinder button
    float cylinderX1 = coneX2 + buttonGap;
    float cylinderX2 = cylinderX1 + buttonWidth;
    bool hoverCylinder = isInsideNDC(mouseX, mouseY, width, height, cylinderX1, row2Y - buttonHeight, cylinderX2, row2Y);
    drawRect(cylinderX1, row2Y - buttonHeight, cylinderX2, row2Y, 0.3f + (hoverCylinder ? 0.1f : 0.0f), 0.3f + (hoverCylinder ? 0.1f : 0.0f), 0.3f + (hoverCylinder ? 0.1f : 0.0f));
    drawOutlineRect(cylinderX1, row2Y - buttonHeight, cylinderX2, row2Y, 0.0f, 0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText("Cylinder", cylinderX1 + pxToNDCx(15, width), row2Y - pxToNDCy(35, height), GLUT_BITMAP_HELVETICA_12);
    drawText("[generate", cylinderX1 + pxToNDCx(15, width), row2Y - pxToNDCy(55, height), GLUT_BITMAP_HELVETICA_12);
    drawText("cylinder]", cylinderX1 + pxToNDCx(15, width), row2Y - pxToNDCy(70, height), GLUT_BITMAP_HELVETICA_12);

    // Row 3: Torus and Pyramid
    float row3Y = row2Y - buttonHeight - rowGap;

    // Torus button
    float torusX1 = panelX1 + pxToNDCx(10, width);
    float torusX2 = torusX1 + buttonWidth;
    bool hoverTorus = isInsideNDC(mouseX, mouseY, width, height, torusX1, row3Y - buttonHeight, torusX2, row3Y);
    drawRect(torusX1, row3Y - buttonHeight, torusX2, row3Y, 0.3f + (hoverTorus ? 0.1f : 0.0f), 0.3f + (hoverTorus ? 0.1f : 0.0f), 0.3f + (hoverTorus ? 0.1f : 0.0f));
    drawOutlineRect(torusX1, row3Y - buttonHeight, torusX2, row3Y, 0.0f, 0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText("Torus", torusX1 + pxToNDCx(25, width), row3Y - pxToNDCy(35, height), GLUT_BITMAP_HELVETICA_12);
    drawText("[generate", torusX1 + pxToNDCx(15, width), row3Y - pxToNDCy(55, height), GLUT_BITMAP_HELVETICA_12);
    drawText("torus]", torusX1 + pxToNDCx(25, width), row3Y - pxToNDCy(70, height), GLUT_BITMAP_HELVETICA_12);

    // Pyramid button
    float pyramidX1 = torusX2 + buttonGap;
    float pyramidX2 = pyramidX1 + buttonWidth;
    bool hoverPyramid = isInsideNDC(mouseX, mouseY, width, height, pyramidX1, row3Y - buttonHeight, pyramidX2, row3Y);
    drawRect(pyramidX1, row3Y - buttonHeight, pyramidX2, row3Y, 0.3f + (hoverPyramid ? 0.1f : 0.0f), 0.3f + (hoverPyramid ? 0.1f : 0.0f), 0.3f + (hoverPyramid ? 0.1f : 0.0f));
    drawOutlineRect(pyramidX1, row3Y - buttonHeight, pyramidX2, row3Y, 0.0f, 0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText("Pyramid", pyramidX1 + pxToNDCx(20, width), row3Y - pxToNDCy(35, height), GLUT_BITMAP_HELVETICA_12);
    drawText("[generate", pyramidX1 + pxToNDCx(15, width), row3Y - pxToNDCy(55, height), GLUT_BITMAP_HELVETICA_12);
    drawText("pyramid]", pyramidX1 + pxToNDCx(17, width), row3Y - pxToNDCy(70, height), GLUT_BITMAP_HELVETICA_12);

    // Disable scissor test after drawing panel content
    glDisable(GL_SCISSOR_TEST);

    // Draw scroll bar for shapes panel (outside scissor test so it's always visible)
    if (maxScroll > 0) {
        float scrollBarWidth = pxToNDCx(8, width);
        float scrollBarX = panelX1 + panelWidth - scrollBarWidth - pxToNDCx(5, width);
        float scrollBarHeight = panelHeight - pxToNDCy(15, height);
        float scrollThumbHeight = scrollBarHeight * (panelHeight / contentHeight);
        float scrollProgress = leftPanelShapesScroll / maxScroll;
        float scrollThumbY = panelY1 + panelHeight - pxToNDCy(10, height) - scrollThumbHeight - (scrollProgress * (scrollBarHeight - scrollThumbHeight));

        // Scroll bar background
        drawRect(scrollBarX, panelY1 + pxToNDCy(10, height), scrollBarX + scrollBarWidth, panelY1 + panelHeight - pxToNDCy(10, height), 0.3f, 0.3f, 0.3f);
        // Scroll thumb
        drawRect(scrollBarX, scrollThumbY, scrollBarX + scrollBarWidth, scrollThumbY + scrollThumbHeight, 0.6f, 0.6f, 0.6f);
    }
}

// Draw Textures Panel with scissor test
void drawTexturesPanel(float panelX1, float panelY1, float panelWidth, float panelHeight, int width, int height) {
    // Convert NDC coordinates to pixel coordinates for scissor
    int panelXPx = (int)((panelX1 + 1.0f) * 0.5f * width);
    int panelYPx = (int)((1.0f - (panelY1 + panelHeight)) * 0.5f * height);
    int panelWidthPx = (int)(panelWidth * 0.5f * width);
    int panelHeightPx = (int)(panelHeight * 0.5f * height);

    // Enable scissor test for this panel
    glEnable(GL_SCISSOR_TEST);
    setScissor(panelXPx, panelYPx, panelWidthPx, panelHeightPx, height);

    // Panel background
    drawRect(panelX1, panelY1, panelX1 + panelWidth, panelY1 + panelHeight, 0.4f, 0.4f, 0.4f);
    drawOutlineRect(panelX1, panelY1, panelX1 + panelWidth, panelY1 + panelHeight, 0.0f, 0.0f, 0.0f, 2.0f);

    // Title
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText("Textures", panelX1 + pxToNDCx(10, width), panelY1 + panelHeight - pxToNDCy(25, height), GLUT_BITMAP_HELVETICA_18);

    // Texture button dimensions - properly sized to fit within panel
    float buttonWidth = (panelWidth - pxToNDCx(40, width)) / 2.0f; // Reduced for scroll bar
    float buttonHeight = pxToNDCy(75, height); // Adjusted height
    float buttonGap = pxToNDCx(8, width); // Gap between buttons

    // Calculate content height (3 rows of buttons with gaps)
    float rowGap = pxToNDCy(10, height);
    float contentHeight = (buttonHeight * 3) + (rowGap * 2);
    float maxScroll = std::max(0.0f, contentHeight - panelHeight);

    // Apply scroll offset
    float scrollOffset = std::max(0.0f, std::min(maxScroll, -leftPanelTexturesScroll));

    // Row 1: Texture 1 and 2
    float row1Y = panelY1 + panelHeight - pxToNDCy(50, height) + scrollOffset;

    // Texture 1 button
    float tex1X1 = panelX1 + pxToNDCx(10, width);
    float tex1X2 = tex1X1 + buttonWidth;
    bool hoverTex1 = isInsideNDC(mouseX, mouseY, width, height, tex1X1, row1Y - buttonHeight, tex1X2, row1Y);
    drawRect(tex1X1, row1Y - buttonHeight, tex1X2, row1Y, 0.3f + (hoverTex1 ? 0.1f : 0.0f), 0.3f + (hoverTex1 ? 0.1f : 0.0f), 0.3f + (hoverTex1 ? 0.1f : 0.0f));
    drawOutlineRect(tex1X1, row1Y - buttonHeight, tex1X2, row1Y, 0.0f, 0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText("[peanut.png]", tex1X1 + pxToNDCx(8, width), row1Y - pxToNDCy(45, height), GLUT_BITMAP_HELVETICA_12);

    // Texture 2 button
    float tex2X1 = tex1X2 + buttonGap;
    float tex2X2 = tex2X1 + buttonWidth;
    bool hoverTex2 = isInsideNDC(mouseX, mouseY, width, height, tex2X1, row1Y - buttonHeight, tex2X2, row1Y);
    drawRect(tex2X1, row1Y - buttonHeight, tex2X2, row1Y, 0.3f + (hoverTex2 ? 0.1f : 0.0f), 0.3f + (hoverTex2 ? 0.1f : 0.0f), 0.3f + (hoverTex2 ? 0.1f : 0.0f));
    drawOutlineRect(tex2X1, row1Y - buttonHeight, tex2X2, row1Y, 0.0f, 0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText("[peanut2.png]", tex2X1 + pxToNDCx(8, width), row1Y - pxToNDCy(45, height), GLUT_BITMAP_HELVETICA_12);

    // Row 2: Texture 3 and 4
    float row2Y = row1Y - buttonHeight - rowGap;

    // Texture 3 button
    float tex3X1 = panelX1 + pxToNDCx(10, width);
    float tex3X2 = tex3X1 + buttonWidth;
    bool hoverTex3 = isInsideNDC(mouseX, mouseY, width, height, tex3X1, row2Y - buttonHeight, tex3X2, row2Y);
    drawRect(tex3X1, row2Y - buttonHeight, tex3X2, row2Y, 0.3f + (hoverTex3 ? 0.1f : 0.0f), 0.3f + (hoverTex3 ? 0.1f : 0.0f), 0.3f + (hoverTex3 ? 0.1f : 0.0f));
    drawOutlineRect(tex3X1, row2Y - buttonHeight, tex3X2, row2Y, 0.0f, 0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText("[peanut3.png]", tex3X1 + pxToNDCx(8, width), row2Y - pxToNDCy(45, height), GLUT_BITMAP_HELVETICA_12);

    // Texture 4 button
    float tex4X1 = tex3X2 + buttonGap;
    float tex4X2 = tex4X1 + buttonWidth;
    bool hoverTex4 = isInsideNDC(mouseX, mouseY, width, height, tex4X1, row2Y - buttonHeight, tex4X2, row2Y);
    drawRect(tex4X1, row2Y - buttonHeight, tex4X2, row2Y, 0.3f + (hoverTex4 ? 0.1f : 0.0f), 0.3f + (hoverTex4 ? 0.1f : 0.0f), 0.3f + (hoverTex4 ? 0.1f : 0.0f));
    drawOutlineRect(tex4X1, row2Y - buttonHeight, tex4X2, row2Y, 0.0f, 0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText("[peanut4.png]", tex4X1 + pxToNDCx(8, width), row2Y - pxToNDCy(45, height), GLUT_BITMAP_HELVETICA_12);

    // Row 3: Texture 5 and 6
    float row3Y = row2Y - buttonHeight - rowGap;

    // Texture 5 button
    float tex5X1 = panelX1 + pxToNDCx(10, width);
    float tex5X2 = tex5X1 + buttonWidth;
    bool hoverTex5 = isInsideNDC(mouseX, mouseY, width, height, tex5X1, row3Y - buttonHeight, tex5X2, row3Y);
    drawRect(tex5X1, row3Y - buttonHeight, tex5X2, row3Y, 0.3f + (hoverTex5 ? 0.1f : 0.0f), 0.3f + (hoverTex5 ? 0.1f : 0.0f), 0.3f + (hoverTex5 ? 0.1f : 0.0f));
    drawOutlineRect(tex5X1, row3Y - buttonHeight, tex5X2, row3Y, 0.0f, 0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText("[peanut5.png]", tex5X1 + pxToNDCx(8, width), row3Y - pxToNDCy(45, height), GLUT_BITMAP_HELVETICA_12);

    // Texture 6 button
    float tex6X1 = tex5X2 + buttonGap;
    float tex6X2 = tex6X1 + buttonWidth;
    bool hoverTex6 = isInsideNDC(mouseX, mouseY, width, height, tex6X1, row3Y - buttonHeight, tex6X2, row3Y);
    drawRect(tex6X1, row3Y - buttonHeight, tex6X2, row3Y, 0.3f + (hoverTex6 ? 0.1f : 0.0f), 0.3f + (hoverTex6 ? 0.1f : 0.0f), 0.3f + (hoverTex6 ? 0.1f : 0.0f));
    drawOutlineRect(tex6X1, row3Y - buttonHeight, tex6X2, row3Y, 0.0f, 0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText("[peanut6.png]", tex6X1 + pxToNDCx(8, width), row3Y - pxToNDCy(45, height), GLUT_BITMAP_HELVETICA_12);

    // Disable scissor test after drawing panel content
    glDisable(GL_SCISSOR_TEST);

    // Draw scroll bar for textures panel (outside scissor test so it's always visible)
    if (maxScroll > 0) {
        float scrollBarWidth = pxToNDCx(8, width);
        float scrollBarX = panelX1 + panelWidth - scrollBarWidth - pxToNDCx(5, width);
        float scrollBarHeight = panelHeight - pxToNDCy(15, height);
        float scrollThumbHeight = scrollBarHeight * (panelHeight / contentHeight);
        float scrollProgress = leftPanelTexturesScroll / maxScroll;
        float scrollThumbY = panelY1 + panelHeight - pxToNDCy(10, height) - scrollThumbHeight - (scrollProgress * (scrollBarHeight - scrollThumbHeight));

        // Scroll bar background
        drawRect(scrollBarX, panelY1 + pxToNDCy(10, height), scrollBarX + scrollBarWidth, panelY1 + panelHeight - pxToNDCy(10, height), 0.3f, 0.3f, 0.3f);
        // Scroll thumb
        drawRect(scrollBarX, scrollThumbY, scrollBarX + scrollBarWidth, scrollThumbY + scrollThumbHeight, 0.6f, 0.6f, 0.6f);
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

    // Get the primary monitor and its video mode
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);

    // Create fullscreen window
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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    // Initialize GLUT (only for font rendering). glutInit requires argc/argv; this is fine.
    int argc = 0;
    char** argv = nullptr;
    glutInit(&argc, argv);

    glClearColor(0.12f, 0.12f, 0.15f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        if (width < MIN_WINDOW_WIDTH + leftBarWidth + rightBarWidth) {
            width = MIN_WINDOW_WIDTH + leftBarWidth + rightBarWidth;
        }

        // NDC positions for bar edges
        float leftEdgeNDC = -1.0f + (2.0f * leftBarWidth / width);
        float rightEdgeNDC = 1.0f - (2.0f * rightBarWidth / width);
        float topEdgeNDC = 1.0f - (2.0f * topBarHeight / height);

        // handle sizes in NDC
        float handleNDC_W = 2.0f * HANDLE_PX / width;

        // Draw side bars and top bar
        drawRect(-1.0f, -1.0f, leftEdgeNDC, topEdgeNDC, 0.5f, 0.5f, 0.5f); // Left sidebar
        drawRect(rightEdgeNDC, -1.0f, 1.0f, topEdgeNDC, 0.5f, 0.5f, 0.5f); // Right sidebar
        drawRect(-1.0f, topEdgeNDC, 1.0f, 1.0f, 0.45f, 0.45f, 0.45f); // Top bar

        // handles (left/right only)
        drawRect(leftEdgeNDC - handleNDC_W, -1.0f, leftEdgeNDC, topEdgeNDC, 0.7f, 0.7f, 0.7f);
        drawRect(rightEdgeNDC, -1.0f, rightEdgeNDC + handleNDC_W, topEdgeNDC, 0.7f, 0.7f, 0.7f);

        // UI layout metrics (pixel based -> convert to NDC)
        float marginPx = 10.0f;
        float marginX = pxToNDCx((int)marginPx, width);

        float topBarY2 = 1.0f;

        // Left-most "Blender Life" box
        float blenderWpx = 120.0f;
        float blenderHpx = 48.0f;
        float blenderX1 = -1.0f + marginX;
        float blenderX2 = blenderX1 + pxToNDCx((int)blenderWpx, width);
        float blenderH = pxToNDCy((int)blenderHpx, height);
        float blenderY2 = topBarY2 - pxToNDCy(8, height);
        float blenderY1 = blenderY2 - blenderH;

        // Draw Blender Life box (outline + fill slightly darker than top bar)
        drawRect(blenderX1, blenderY1, blenderX2, blenderY2, 0.38f, 0.38f, 0.38f);
        drawOutlineRect(blenderX1, blenderY1, blenderX2, blenderY2, 0.0f, 0.0f, 0.0f, 2.0f);

        // text "Blender Life" in red centered
        glColor3f(1.0f, 0.0f, 0.0f);
        float textX = (blenderX1 + blenderX2) * 0.5f - 0.06f;
        float textY = (blenderY1 + blenderY2) * 0.5f - 0.01f;
        drawText("Blender Life", textX, textY, GLUT_BITMAP_HELVETICA_18);

        // To the right of blender box: stack Save (top) and Save as (below),
        // and under them Undo (left) and Redo (right).
        float groupX1 = blenderX2 + marginX;
        float groupX2 = groupX1 + pxToNDCx((int)(140), width);

        // vertical positions: make vertical stack fit
        float buttonHpx = 28.0f;
        float saveH = pxToNDCy((int)buttonHpx, height);
        float gapV = pxToNDCy(6, height);
        float save1Y2 = blenderY2;
        float save1Y1 = save1Y2 - saveH;
        float save2Y2 = save1Y1 - gapV;
        float save2Y1 = save2Y2 - saveH;

        // Draw Save button
        bool hoverSave = isInsideNDC(mouseX, mouseY, width, height, groupX1, save1Y1, groupX2, save1Y2);
        drawRect(groupX1, save1Y1, groupX2, save1Y2, 0.32f + (hoverSave ? 0.08f : 0.0f), 0.32f + (hoverSave ? 0.08f : 0.0f), 0.32f + (hoverSave ? 0.08f : 0.0f));
        drawOutlineRect(groupX1, save1Y1, groupX2, save1Y2, 0,0,0,1.5f);
        glColor3f(1,1,1);
        drawText("Save", groupX1 + 0.02f, (save1Y1 + save1Y2) * 0.5f - 0.01f, GLUT_BITMAP_HELVETICA_18);

        // Draw Save as button
        bool hoverSaveAs = isInsideNDC(mouseX, mouseY, width, height, groupX1, save2Y1, groupX2, save2Y2);
        drawRect(groupX1, save2Y1, groupX2, save2Y2, 0.32f + (hoverSaveAs ? 0.08f : 0.0f), 0.32f + (hoverSaveAs ? 0.08f : 0.0f), 0.32f + (hoverSaveAs ? 0.08f : 0.0f));
        drawOutlineRect(groupX1, save2Y1, groupX2, save2Y2, 0,0,0,1.5f);
        glColor3f(1,1,1);
        drawText("Save as", groupX1 + 0.01f, (save2Y1 + save2Y2)*0.5f - 0.01f, GLUT_BITMAP_HELVETICA_18);

        // Under those two: Undo (left) and Redo (right)
        float underY2 = save2Y1 - gapV - 0.0f;
        float underH = saveH;
        float underY1 = underY2 - underH;
        float halfW = (groupX2 - groupX1 - marginX) * 0.5f;
        float undoX1 = groupX1;
        float undoX2 = groupX1 + halfW - marginX*0.5f;
        float redoX1 = undoX2 + marginX*0.5f;
        float redoX2 = groupX2;
        bool hoverUndo = isInsideNDC(mouseX, mouseY, width, height, undoX1, underY1, undoX2, underY2);
        bool hoverRedo = isInsideNDC(mouseX, mouseY, width, height, redoX1, underY1, redoX2, underY2);
        drawRect(undoX1, underY1, undoX2, underY2, 0.32f + (hoverUndo ? 0.08f : 0.0f), 0.32f + (hoverUndo ? 0.08f : 0.0f), 0.32f + (hoverUndo ? 0.08f : 0.0f));
        drawOutlineRect(undoX1, underY1, undoX2, underY2, 0,0,0,1.0f);
        glColor3f(1,1,1);
        drawText("Undo", undoX1 + 0.01f, (underY1 + underY2)*0.5f - 0.01f, GLUT_BITMAP_HELVETICA_18);

        drawRect(redoX1, underY1, redoX2, underY2, 0.32f + (hoverRedo ? 0.08f : 0.0f), 0.32f + (hoverRedo ? 0.08f : 0.0f), 0.32f + (hoverRedo ? 0.08f : 0.0f));
        drawOutlineRect(redoX1, underY1, redoX2, underY2, 0,0,0,1.0f);
        glColor3f(1,1,1);
        drawText("Redo", redoX1 + 0.01f, (underY1 + underY2)*0.5f - 0.01f, GLUT_BITMAP_HELVETICA_18);

        // Center area: Project Name placeholder above New Project button
        float centerWpx = 300.0f;
        float centerX1 = -pxToNDCx((int)(centerWpx/2), width);
        float centerX2 = pxToNDCx((int)(centerWpx/2), width);

        // Project Name placeholder rectangle (thin)
        float pnameHpx = 20.0f;
        float pnameH = pxToNDCy((int)pnameHpx, height);
        float pnameY2 = blenderY2;
        float pnameY1 = pnameY2 - pnameH - pxToNDCy(6, height);
        drawRect(centerX1, pnameY1, centerX2, pnameY2, 0.9f, 0.9f, 0.9f);
        drawOutlineRect(centerX1, pnameY1, centerX2, pnameY2, 0,0,0,1.0f);
        glColor3f(0.2f,0.2f,0.2f);
        drawText("Project Name", centerX1 + 0.03f, (pnameY1 + pnameY2)*0.5f - 0.01f, GLUT_BITMAP_HELVETICA_18);

        // New Project button under placeholder
        float npWpx = 150.0f;
        float npX1 = -pxToNDCx((int)(npWpx/2), width);
        float npX2 = pxToNDCx((int)(npWpx/2), width);
        float npH = pxToNDCy((int)buttonHpx, height);
        float npY2 = pnameY1 - pxToNDCy(8, height);
        float npY1 = npY2 - npH;
        bool hoverNewProj = isInsideNDC(mouseX, mouseY, width, height, npX1, npY1, npX2, npY2);
        drawRect(npX1, npY1, npX2, npY2, 0.32f + (hoverNewProj ? 0.08f : 0.0f), 0.32f + (hoverNewProj ? 0.08f : 0.0f), 0.32f + (hoverNewProj ? 0.08f : 0.0f));
        drawOutlineRect(npX1, npY1, npX2, npY2, 0,0,0,1.0f);
        glColor3f(1,1,1);
        drawText("New Project", npX1 + 0.01f, (npY1 + npY2)*0.5f - 0.01f, GLUT_BITMAP_HELVETICA_18);

        // Camera button at very right with rounded-ish square and circle lens
        float camSizePx = 48.0f;
        float camSize = pxToNDCx((int)camSizePx, width);
        float camX2 = 1.0f - pxToNDCx(50, width);
        float camX1 = camX2 - camSize;
        float camY2 = blenderY2;
        float camY1 = camY2 - pxToNDCy((int)camSizePx, height);
        bool hoverCam = isInsideNDC(mouseX, mouseY, width, height, camX1, camY1, camX2, camY2);
        float camExpandPx = hoverCam ? 6.0f : 0.0f;
        float camEx = pxToNDCx((int)camExpandPx, width);

        // Camera rounded square (approx with rect)
        drawRect(camX1 - camEx, camY1 - camEx, camX2 + camEx, camY2 + camEx, 0.3f, 0.3f, 0.3f);
        drawOutlineRect(camX1 - camEx, camY1 - camEx, camX2 + camEx, camY2 + camEx, 0,0,0,1.0f);

        // Lens circle in middle
        float camCx = (camX1 + camX2) * 0.5f;
        float camCy = (camY1 + camY2) * 0.5f;
        drawCircle(camCx, camCy, std::min(camSize, pxToNDCy((int)camSizePx, height)) * 0.25f + (hoverCam ? camEx*0.5f : 0.0f), 30, 0.12f, 0.12f, 0.12f);

        // Text under camera: two lines centered under camera
        glColor3f(1,1,1);
        float textLine1Y = camY1 - pxToNDCy(14, height);
        float textLine2Y = textLine1Y - pxToNDCy(22, height);
        drawText("Take a", camCx - 0.05f, textLine1Y, GLUT_BITMAP_HELVETICA_12);
        drawText("Screenshot", camCx - 0.09f, textLine2Y, GLUT_BITMAP_HELVETICA_12);

        // COLORS PANEL - Positioned between Project Name and Camera button
        float colorsPanelWpx = 220.0f;
        float colorsPanelHpx = 80.0f; // Reduced height to fit in top bar
        float colorsPanelW = pxToNDCx((int)colorsPanelWpx, width);
        float colorsPanelH = pxToNDCy((int)colorsPanelHpx, height);

        // Calculate available space between Project Name and Camera
        float projectNameRight = centerX2;
        float cameraLeft = camX1;
        float availableSpace = cameraLeft - projectNameRight;

        // Position colors panel in the center of available space
        float colorsX1 = projectNameRight + pxToNDCx(10, width);
        float colorsX2 = colorsX1 + colorsPanelW;

        // Ensure it doesn't overlap with camera
        if (colorsX2 > cameraLeft - pxToNDCx(10, width)) {
            colorsX2 = cameraLeft - pxToNDCx(10, width);
            colorsX1 = colorsX2 - colorsPanelW;
        }

        // Position vertically in top bar
        float colorsY2 = topBarY2 - pxToNDCy(8, height);
        float colorsY1 = colorsY2 - colorsPanelH;

        // Draw background and outline
        drawRect(colorsX1, colorsY1, colorsX2, colorsY2, 0.4f, 0.4f, 0.4f);
        drawOutlineRect(colorsX1, colorsY1, colorsX2, colorsY2, 0, 0, 0, 2.0f);

        // Title "Colors" at top
        glColor3f(1, 1, 1);
        drawText("Colors", colorsX1 + 0.02f, colorsY2 - pxToNDCy(18, height), GLUT_BITMAP_HELVETICA_12);

        // Swatches row (6 perfect squares) - compact layout
        int numColors = 6;
        float swatchPx = 20.0f; // Smaller swatches
        float swatchGapPx = 5.0f; // Smaller gap
        float swatchSize = pxToNDCx((int)swatchPx, width);
        float swatchGap = pxToNDCx((int)swatchGapPx, width);
        float startSwX = colorsX1 + pxToNDCx(10, width);
        float swYTop = colorsY2 - pxToNDCy(30, height); // Moved up
        float swYBottom = swYTop - swatchSize;

        // Fixed struct definition
        struct Color { float r, g, b; };
        Color colorArr[6] = {{0,0,0}, {1,1,1}, {1,0,0}, {0,1,0}, {0,0,1}, {1,0,1}};
        for (int i = 0; i < numColors; i++) {
            float sx1 = startSwX + i * (swatchSize + swatchGap);
            float sx2 = sx1 + swatchSize;
            bool hover = isInsideNDC(mouseX, mouseY, width, height, sx1, swYBottom, sx2, swYTop);
            float expandPx = hover ? 2.0f : 0.0f;
            float ex = pxToNDCx((int)expandPx, width);
            drawRect(sx1 - ex, swYBottom - ex, sx2 + ex, swYTop + ex, colorArr[i].r, colorArr[i].g, colorArr[i].b);
            drawOutlineRect(sx1 - ex, swYBottom - ex, sx2 + ex, swYTop + ex, 0, 0, 0, 1.0f);
        }

        // "V" button and "Shades" box in a compact row below swatches
        float vBtnPx = 22.0f;
        float vW = pxToNDCx((int)vBtnPx, width);
        float vX1 = startSwX;
        float vX2 = vX1 + vW;
        float vY2 = swYBottom - pxToNDCy(5, height);
        float vY1 = vY2 - pxToNDCy((int)vBtnPx, height);
        bool hoverV = isInsideNDC(mouseX, mouseY, width, height, vX1, vY1, vX2, vY2);
        drawRect(vX1, vY1, vX2, vY2, hoverV ? 0.6f : 0.25f, hoverV ? 0.6f : 0.25f, hoverV ? 0.6f : 0.25f);
        drawOutlineRect(vX1, vY1, vX2, vY2, 0, 0, 0, 1.0f);
        glColor3f(1,1,1);
        drawText("V", vX1 + 0.005f, (vY1 + vY2)*0.5f - 0.01f, GLUT_BITMAP_HELVETICA_12);

        // Shades box to the right of V
        float shadesWpx = 80.0f;
        float shadesHpx = 22.0f;
        float shadesW = pxToNDCx((int)shadesWpx, width);
        float shadesH = pxToNDCy((int)shadesHpx, height);
        float shadesX1 = vX2 + pxToNDCx(5, width);
        float shadesX2 = shadesX1 + shadesW;
        float shadesY2 = vY2;
        float shadesY1 = vY2 - shadesH;
        isInsideNDC(mouseX, mouseY, width, height, shadesX1, shadesY1, shadesX2, shadesY2);
        drawRect(shadesX1, shadesY1, shadesX2, shadesY2, 0.3f, 0.3f, 0.3f);
        drawOutlineRect(shadesX1, shadesY1, shadesX2, shadesY2, 0, 0, 0, 1.0f);
        glColor3f(1,1,1);
        drawText("Shades", shadesX1 + 0.01f, (shadesY1 + shadesY2)*0.5f - 0.01f, GLUT_BITMAP_HELVETICA_12);

        // LEFT SIDEBAR: Shapes and Textures panels with scissor test
        float leftPanelWidth = pxToNDCx((int)(leftBarWidth * 0.85f), width);
        float leftPanelHeight = pxToNDCy(310, height); // Increased from 280 to 310 (30px taller)
        float leftPanelGap = pxToNDCy(20, height);

        // Start from bottom and work upwards
        float leftVerticalOffset = pxToNDCy(5, height);

        // Textures panel at the bottom
        float texturesPanelY = -1.0f + leftPanelTexturesScroll + leftVerticalOffset;
        drawTexturesPanel(-1.0f + pxToNDCx(5, width), texturesPanelY,
                         leftPanelWidth, leftPanelHeight, width, height);

        // Shapes panel above textures (with proper gap)
        float shapesPanelY = texturesPanelY + leftPanelHeight + leftPanelGap + leftPanelShapesScroll;
        drawShapesPanel(-1.0f + pxToNDCx(5, width), shapesPanelY,
                       leftPanelWidth, leftPanelHeight, width, height);

        // RIGHT SIDEBAR: Vector panels
        float panelWidth = pxToNDCx((int)(rightBarWidth * 0.85f), width);
        float panelHeight = pxToNDCy(180, height);
        float panelGap = pxToNDCy(15, height);

        // Calculate total height of all panels
        float totalPanelsHeight = (panelHeight * 3) + (panelGap * 2);

        // Start from bottom and work upwards
        float rightVerticalOffset = pxToNDCy(50, height); // 50 pixels up
        float panelStartY = -1.0f + totalPanelsHeight + rightPanelScroll + rightVerticalOffset;

        // Rotate panel at the very bottom
        drawTransformPanel(rightEdgeNDC + pxToNDCx(5, width), -1.0f + rightPanelScroll + rightVerticalOffset,
                          panelWidth, panelHeight,
                          "Rotation", rotate, width, height);

        // Scale panel above rotate
        float scalePanelY = -1.0f + panelHeight + panelGap + rightPanelScroll + rightVerticalOffset;
        drawTransformPanel(rightEdgeNDC + pxToNDCx(5, width), scalePanelY,
                          panelWidth, panelHeight,
                          "Scaling", scale, width, height);

        // Translate panel at the top (closest to top bar)
        float translatePanelY = -1.0f + (panelHeight * 2) + (panelGap * 2) + rightPanelScroll + rightVerticalOffset;
        drawTransformPanel(rightEdgeNDC + pxToNDCx(5, width), translatePanelY,
                          panelWidth, panelHeight,
                          "Translate", translate, width, height);

        // Scroll bar for right panel
        float scrollBarWidth = pxToNDCx(8, width);
        float scrollBarX = 1.0f - scrollBarWidth; // Right edge of screen
        float scrollBarHeight = topEdgeNDC - (-1.0f); // From bottom to top bar
        float scrollThumbHeight = scrollBarHeight * 0.3f;
        float maxScroll = totalPanelsHeight - (topEdgeNDC - (-1.0f));
        if (maxScroll < 0) maxScroll = 0;
        float scrollProgress = 0.0f;
        if (maxScroll > 0) {
            scrollProgress = std::max(0.0f, std::min(1.0f, -rightPanelScroll / maxScroll));
        }
        float scrollThumbY = -1.0f + (1.0f - scrollProgress) * scrollBarHeight;

        // Scroll bar background
        drawRect(scrollBarX, -1.0f, scrollBarX + scrollBarWidth, topEdgeNDC, 0.3f, 0.3f, 0.3f);
        // Scroll thumb
        drawRect(scrollBarX, scrollThumbY - scrollThumbHeight, scrollBarX + scrollBarWidth, scrollThumbY, 0.6f, 0.6f, 0.6f);

        // Big central canvas area border
        float canvasX1 = leftEdgeNDC + pxToNDCx(6, width);
        float canvasX2 = rightEdgeNDC - pxToNDCx(6, width);
        float canvasY1 = -1.0f;
        float canvasY2 = topEdgeNDC - pxToNDCy(6, height);
        drawRect(canvasX1, canvasY1, canvasX2, canvasY2, 0.12f, 0.12f, 0.15f);
        drawOutlineRect(canvasX1, canvasY1, canvasX2, canvasY2, 0.2f, 0.2f, 0.2f, 1.0f);

        // Swap buffers & poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// Callbacks & input
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;
    glViewport(0,0,width,height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    float leftEdge = leftBarWidth;
    float rightEdge = width - rightBarWidth;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (xpos >= leftEdge - HANDLE_PX && xpos <= leftEdge) resizingLeft = true;
        else if (xpos >= rightEdge && xpos <= rightEdge + HANDLE_PX) resizingRight = true;
        lastX = xpos;
        lastY = ypos;
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        resizingLeft = resizingRight = false;
    }
    (void)mods;
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    mouseX = xpos;
    mouseY = ypos;
    if (!resizingLeft && !resizingRight) return;

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    double dx = xpos - lastX;
    lastX = xpos;
    lastY = ypos;

    if (resizingLeft) {
        leftBarWidth = std::max(MIN_LEFT_BAR_WIDTH, std::min((float)(leftBarWidth + dx), width - rightBarWidth - MIN_WINDOW_WIDTH));
    }
    else if (resizingRight) {
        rightBarWidth = std::max(MIN_RIGHT_BAR_WIDTH, std::min((float)(rightBarWidth - dx), width - leftBarWidth - MIN_WINDOW_WIDTH));
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)window;
    (void)xoffset;

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Get mouse position to determine which panel to scroll
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);

    float leftEdgeNDC = -1.0f + (2.0f * leftBarWidth / width);

    // Check if mouse is over left sidebar
    if (mx < leftBarWidth) {
        // Check if mouse is over shapes or textures area (simple vertical split)
        if (my < height / 2) {
            // Scroll shapes panel
            leftPanelShapesScroll += yoffset * 20.0f;
            float maxShapesScroll = 400.0f;
            leftPanelShapesScroll = std::max(-maxShapesScroll, std::min(0.0f, leftPanelShapesScroll));
        } else {
            // Scroll textures panel
            leftPanelTexturesScroll += yoffset * 20.0f;
            float maxTexturesScroll = 400.0f;
            leftPanelTexturesScroll = std::max(-maxTexturesScroll, std::min(0.0f, leftPanelTexturesScroll));
        }
    } else {
        // Scroll right panel
        rightPanelScroll += yoffset * 20.0f;
        float maxScroll = 400.0f;
        rightPanelScroll = std::max(-maxScroll, std::min(0.0f, rightPanelScroll));
    }
}