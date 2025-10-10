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
const float M_PI = (float)M_PI_DOUBLE; // keep your float M_PI usage

// Window settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 600;

// Bar variables (pixel units)
float leftBarWidth = 150.0f;
float rightBarWidth = 150.0f;
float topBarHeight = 110.0f; // taller to fit UI

// Handle size (pixels)
const float HANDLE_PX = 10.0f;

// State
bool resizingLeft = false;
bool resizingRight = false;
bool resizingTop = false;
double lastX = 0.0, lastY = 0.0;
double mouseX = 0.0, mouseY = 0.0;

// Helpers
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

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

int main() {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "BlenderLite", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

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

        // NDC positions for bar edges
        float leftEdgeNDC = -1.0f + (2.0f * leftBarWidth / width);
        float rightEdgeNDC = 1.0f - (2.0f * rightBarWidth / width);
        float topEdgeNDC = 1.0f - (2.0f * topBarHeight / height);

        // handle sizes in NDC
        float handleNDC_W = 2.0f * HANDLE_PX / width;
        float handleNDC_H = 2.0f * HANDLE_PX / height;
        float topLimitNDC = topEdgeNDC;

        // Draw side bars and top bar
        drawRect(-1.0f, -1.0f, leftEdgeNDC, topLimitNDC, 0.5f, 0.5f, 0.5f);
        drawRect(rightEdgeNDC, -1.0f, 1.0f, topLimitNDC, 0.5f, 0.5f, 0.5f);
        drawRect(-1.0f, topEdgeNDC, 1.0f, 1.0f, 0.45f, 0.45f, 0.45f);

        // handles (unchanged)
        drawRect(leftEdgeNDC - handleNDC_W, -1.0f, leftEdgeNDC, topLimitNDC, 0.7f, 0.7f, 0.7f);
        drawRect(rightEdgeNDC, -1.0f, rightEdgeNDC + handleNDC_W, topLimitNDC, 0.7f, 0.7f, 0.7f);
        drawRect(-1.0f, topEdgeNDC, 1.0f, topEdgeNDC + handleNDC_H, 0.7f, 0.7f, 0.7f);

        // UI layout metrics (pixel based -> convert to NDC)
        float marginPx = 10.0f;
        float marginX = pxToNDCx((int)marginPx, width);
        float marginY = pxToNDCy((int)8, height); // vertical spacing small

        float topBarHeightPx = topBarHeight; // px
        float buttonHpx = 28.0f;
        float buttonWpx = 120.0f;
        float smallBtnWpx = 80.0f;
        float smallBtnHpx = 28.0f;

        // y positions (NDC)
        float topBarY1 = topEdgeNDC;
        float topBarY2 = 1.0f;
        float centerY = (topBarY1 + topBarY2) * 0.5f;

        // Left-most "Blender Lite" box
        float blenderWpx = 120.0f;
        float blenderHpx = 48.0f;
        float blenderX1 = -1.0f + marginX;
        float blenderX2 = blenderX1 + pxToNDCx((int)blenderWpx, width);
        float blenderH = pxToNDCy((int)blenderHpx, height);
        float blenderY2 = topBarY2 - marginY;
        float blenderY1 = blenderY2 - blenderH;

        // Draw Blender Lite box (outline + fill slightly darker than top bar)
        drawRect(blenderX1, blenderY1, blenderX2, blenderY2, 0.38f, 0.38f, 0.38f);
        drawOutlineRect(blenderX1, blenderY1, blenderX2, blenderY2, 0.0f, 0.0f, 0.0f, 2.0f);

        // text "Blender Lite" in red centered
        float textX = (blenderX1 + blenderX2) * 0.5f - 0.06f; // small offset to center in GLUT raster coords
        float textY = (blenderY1 + blenderY2) * 0.5f - 0.01f;
        glColor3f(1.0f, 0.0f, 0.0f);
        drawText("Blender Lite", textX, textY, GLUT_BITMAP_HELVETICA_18);

        // To the right of blender box: stack Save (top) and Save as (below),
        // and under them Undo (left) and Redo (right).
        float groupX1 = blenderX2 + marginX;
        float groupX2 = groupX1 + pxToNDCx((int) (buttonWpx + 10), width);

        // vertical positions: make vertical stack fit
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
        drawRect(centerX1, pnameY1, centerX2, pnameY2, 0.9f, 0.9f, 0.9f); // lighter box for placeholder
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

        // RIGHT SIDE: Colors box + Shades + V button
        float colorsBoxWpx = 240.0f;
        float colorsBoxHpx = 68.0f;
        float colorsBoxW = pxToNDCx((int)colorsBoxWpx, width);
        float colorsBoxH = pxToNDCy((int)colorsBoxHpx, height);
        float colorsX2 = rightEdgeNDC - marginX;
        float colorsX1 = colorsX2 - colorsBoxW;
        float colorsY2 = blenderY2;
        float colorsY1 = colorsY2 - colorsBoxH;

        // Colors title above the color swatches
        glColor3f(1,1,1);
        drawText("Colors", colorsX1 + 0.01f, colorsY2 - pxToNDCy(14, height), GLUT_BITMAP_HELVETICA_18);

        // Draw outline around entire colors+shades area
        drawRect(colorsX1, colorsY1, colorsX2, colorsY2, 0.4f, 0.4f, 0.4f); // slightly darker
        drawOutlineRect(colorsX1, colorsY1, colorsX2, colorsY2, 0,0,0,2.0f);

        // Color swatches row inside
        int numColors = 6;
        float swatchSize = pxToNDCx(22, width);
        float swatchGap = pxToNDCx(8, width);
        float swStartX = colorsX1 + pxToNDCx(12, width);
        float swY1 = colorsY1 + pxToNDCy(12, height);
        float swY2 = swY1 + swatchSize;
        struct C { float r,g,b; };
        C colorArr[6] = { {0,0,0}, {1,1,1}, {1,0,0}, {0,1,0}, {0,0,1}, {1,0,1} };
        for (int i=0;i<numColors;i++){
            float sx1 = swStartX + i*(swatchSize + swatchGap);
            float sx2 = sx1 + swatchSize;
            bool hover = isInsideNDC(mouseX, mouseY, width, height, sx1, swY1, sx2, swY2);
            float expandPx = hover ? 4.0f : 0.0f;
            float ex = pxToNDCx((int)expandPx, width);
            drawRect(sx1 - ex, swY1 - ex, sx2 + ex, swY2 + ex, colorArr[i].r, colorArr[i].g, colorArr[i].b);
            drawOutlineRect(sx1 - ex, swY1 - ex, sx2 + ex, swY2 + ex, 0,0,0,1.0f);
        }

        // Shades rectangle to the right of swatches (same box height)
        float shadesWpx = 110.0f;
        float shadesW = pxToNDCx((int)shadesWpx, width);
        float shadesX2 = colorsX2 - pxToNDCx(12, width);
        float shadesX1 = shadesX2 - shadesW;
        float shadesY1 = swY1;
        float shadesY2 = swY2;
        drawRect(shadesX1, shadesY1, shadesX2, shadesY2, 0.3f, 0.3f, 0.3f);
        drawOutlineRect(shadesX1, shadesY1, shadesX2, shadesY2, 0,0,0,1.0f);
        glColor3f(1,1,1);
        drawText("Shades", shadesX1 + 0.01f, (shadesY1 + shadesY2)*0.5f - 0.01f, GLUT_BITMAP_HELVETICA_18);

        // V square button to the left of Shades
        float vWpx = 28.0f;
        float vW = pxToNDCx((int)vWpx, width);
        float vX2 = shadesX1 - pxToNDCx(8, width);
        float vX1 = vX2 - vW;
        float vY1 = shadesY1;
        float vY2 = shadesY2;
        bool hoverV = isInsideNDC(mouseX, mouseY, width, height, vX1, vY1, vX2, vY2);
        drawRect(vX1, vY1, vX2, vY2, hoverV ? 0.6f : 0.2f, hoverV ? 0.6f : 0.2f, hoverV ? 0.6f : 0.2f);
        drawOutlineRect(vX1, vY1, vX2, vY2, 0,0,0,1.0f);
        glColor3f(1,1,1);
        drawText("V", vX1 + 0.01f, (vY1 + vY2)*0.5f - 0.01f, GLUT_BITMAP_HELVETICA_18);

        // Camera button at very right with rounded-ish square and circle lens
        float camSizePx = 48.0f;
        float camSize = pxToNDCx((int)camSizePx, width);
        float camX2 = 1.0f - marginX;
        float camX1 = camX2 - camSize;
        float camY2 = blenderY2;
        float camY1 = camY2 - pxToNDCy((int)camSizePx, height);
        bool hoverCam = isInsideNDC(mouseX, mouseY, width, height, camX1, camY1, camX2, camY2);
        float camExpandPx = hoverCam ? 6.0f : 0.0f;
        float camEx = pxToNDCx((int)camExpandPx, width);

        // Camera rounded square (approx with rect and a darker inner rect)
        drawRect(camX1 - camEx, camY1 - camEx, camX2 + camEx, camY2 + camEx, 0.3f, 0.3f, 0.3f);
        drawOutlineRect(camX1 - camEx, camY1 - camEx, camX2 + camEx, camY2 + camEx, 0,0,0,1.0f);

        // Lens circle in middle
        float camCx = (camX1 + camX2) * 0.5f;
        float camCy = (camY1 + camY2) * 0.5f;
        drawCircle(camCx, camCy, std::min(camSize, pxToNDCy((int)camSizePx, height)) * 0.25f + (hoverCam ? camEx*0.5f : 0.0f), 30, 0.12f, 0.12f, 0.12f);

        // Text under camera: two lines: "Take a" and "Screenshot"
        glColor3f(1,1,1);
        float textLine1Y = camY1 - pxToNDCy(6, height);
        float textLine2Y = textLine1Y - pxToNDCy(14, height);
        drawText("Take a", camCx - 0.05f, textLine1Y, GLUT_BITMAP_HELVETICA_12);
        drawText("Screenshot", camCx - 0.09f, textLine2Y, GLUT_BITMAP_HELVETICA_12);

        // Swap buffers & poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// Callbacks & input
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
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
    float topEdge = topBarHeight;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // prioritize drag handles
        if (xpos >= leftEdge - HANDLE_PX && xpos <= leftEdge) resizingLeft = true;
        else if (xpos >= rightEdge && xpos <= rightEdge + HANDLE_PX) resizingRight = true;
        else if (ypos >= (topEdge - HANDLE_PX) && ypos <= topEdge) resizingTop = true;
        lastX = xpos;
        lastY = ypos;
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        resizingLeft = resizingRight = resizingTop = false;
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    mouseX = xpos;
    mouseY = ypos;
    if (!resizingLeft && !resizingRight && !resizingTop) return;

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    double dx = xpos - lastX;
    double dy = ypos - lastY;
    lastX = xpos;
    lastY = ypos;

    if (resizingLeft) leftBarWidth = std::max(50.0, std::min(leftBarWidth + dx, (double)width - rightBarWidth - 100.0));
    else if (resizingRight) rightBarWidth = std::max(50.0, std::min(rightBarWidth - dx, (double)width - leftBarWidth - 100.0));
    else if (resizingTop) topBarHeight = std::max(60.0, std::min(topBarHeight + dy, (double)height - 40.0));
}
