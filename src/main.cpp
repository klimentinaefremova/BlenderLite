#include "BlenderLite.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <string>

ApplicationState appState;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void draw3DAxes(float canvasX1, float canvasY1, float canvasX2, float canvasY2, int width, int height) {
    // Save current matrix state
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    // Set up orthographic projection for the canvas area
    float canvasWidth = canvasX2 - canvasX1;
    float canvasHeight = canvasY2 - canvasY1;
    glOrtho(canvasX1, canvasX2, canvasY1, canvasY2, -1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Calculate center of canvas
    float centerX = (canvasX1 + canvasX2) * 0.5f;
    float centerY = (canvasY1 + canvasY2) * 0.5f;

    // Axis length (half the original size)
    float axisLength = 0.1f;
    float arrowSize = 0.015f;

    // Draw X axis (Red) - lighter color if selected
    if (appState.axisSelected[0]) {
        glColor3f(1.0f, 0.5f, 0.5f); // Light red when selected
    } else {
        glColor3f(1.0f, 0.0f, 0.0f); // Normal red
    }
    glBegin(GL_LINES);
    glVertex2f(centerX, centerY);
    glVertex2f(centerX + axisLength, centerY);
    glEnd();

    // X arrow
    glBegin(GL_TRIANGLES);
    glVertex2f(centerX + axisLength, centerY);
    glVertex2f(centerX + axisLength - arrowSize, centerY - arrowSize);
    glVertex2f(centerX + axisLength - arrowSize, centerY + arrowSize);
    glEnd();

    // Draw Y axis (Green) - lighter color if selected
    if (appState.axisSelected[1]) {
        glColor3f(0.5f, 1.0f, 0.5f); // Light green when selected
    } else {
        glColor3f(0.0f, 1.0f, 0.0f); // Normal green
    }
    glBegin(GL_LINES);
    glVertex2f(centerX, centerY);
    glVertex2f(centerX, centerY + axisLength);
    glEnd();

    // Y arrow
    glBegin(GL_TRIANGLES);
    glVertex2f(centerX, centerY + axisLength);
    glVertex2f(centerX - arrowSize, centerY + axisLength - arrowSize);
    glVertex2f(centerX + arrowSize, centerY + axisLength - arrowSize);
    glEnd();

    // Draw Z axis (Blue) - lighter color if selected
    if (appState.axisSelected[2]) {
        glColor3f(0.5f, 0.5f, 1.0f); // Light blue when selected
    } else {
        glColor3f(0.0f, 0.0f, 1.0f); // Normal blue
    }
    glBegin(GL_LINES);
    glVertex2f(centerX, centerY);
    glVertex2f(centerX - axisLength * 0.7f, centerY - axisLength * 0.7f);
    glEnd();

    // Z arrow
    glBegin(GL_TRIANGLES);
    glVertex2f(centerX - axisLength * 0.7f, centerY - axisLength * 0.7f);
    glVertex2f(centerX - axisLength * 0.7f + arrowSize, centerY - axisLength * 0.7f + arrowSize);
    glVertex2f(centerX - axisLength * 0.7f + arrowSize * 0.5f, centerY - axisLength * 0.7f - arrowSize * 0.5f);
    glEnd();

    // Draw axis labels with corresponding colors
    if (appState.axisSelected[0]) {
        glColor3f(1.0f, 0.5f, 0.5f);
    } else {
        glColor3f(1.0f, 0.0f, 0.0f);
    }
    PrimitiveRenderer::drawText("X", centerX + axisLength + 0.01f, centerY - 0.005f, GLUT_BITMAP_HELVETICA_12);

    if (appState.axisSelected[1]) {
        glColor3f(0.5f, 1.0f, 0.5f);
    } else {
        glColor3f(0.0f, 1.0f, 0.0f);
    }
    PrimitiveRenderer::drawText("Y", centerX - 0.005f, centerY + axisLength + 0.01f, GLUT_BITMAP_HELVETICA_12);

    if (appState.axisSelected[2]) {
        glColor3f(0.5f, 0.5f, 1.0f);
    } else {
        glColor3f(0.0f, 0.0f, 1.0f);
    }
    PrimitiveRenderer::drawText("Z", centerX - axisLength * 0.7f - 0.015f, centerY - axisLength * 0.7f - 0.015f, GLUT_BITMAP_HELVETICA_12);

    // Restore matrix state
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

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
    float buttonSize = 0.035f; // Changed back to 0.035 as requested
    float buttonGap = 0.002f;  // Scaled gap
    float marginY = 0.03f;     // Bottom margin

    // Position buttons attached to the left sidebar's right edge
    float startX = leftEdgeNDC; // Directly on the edge of left sidebar
    float startY = canvasY1 + marginY;

    // X button (Red)
    float xBtnX1 = startX;
    float xBtnX2 = xBtnX1 + buttonSize;
    float xBtnY1 = startY;
    float xBtnY2 = xBtnY1 + buttonSize;

    bool hoverX = PrimitiveRenderer::isInsideNDC(appState.mouseX, appState.mouseY, width, height, xBtnX1, xBtnY1, xBtnX2, xBtnY2);

    if (appState.axisSelected[0]) {
        PrimitiveRenderer::drawRect(xBtnX1, xBtnY1, xBtnX2, xBtnY2, 1.0f, 0.5f, 0.5f); // Light red when selected
    } else if (hoverX) {
        PrimitiveRenderer::drawRect(xBtnX1, xBtnY1, xBtnX2, xBtnY2, 1.0f, 0.8f, 0.8f); // Very light red when hovered
    } else {
        PrimitiveRenderer::drawRect(xBtnX1, xBtnY1, xBtnX2, xBtnY2, 1.0f, 0.0f, 0.0f); // Normal red
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
        PrimitiveRenderer::drawRect(yBtnX1, yBtnY1, yBtnX2, yBtnY2, 0.5f, 1.0f, 0.5f); // Light green when selected
    } else if (hoverY) {
        PrimitiveRenderer::drawRect(yBtnX1, yBtnY1, yBtnX2, yBtnY2, 0.8f, 1.0f, 0.8f); // Very light green when hovered
    } else {
        PrimitiveRenderer::drawRect(yBtnX1, yBtnY1, yBtnX2, yBtnY2, 0.0f, 1.0f, 0.0f); // Normal green
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
        PrimitiveRenderer::drawRect(zBtnX1, zBtnY1, zBtnX2, zBtnY2, 0.5f, 0.5f, 1.0f); // Light blue when selected
    } else if (hoverZ) {
        PrimitiveRenderer::drawRect(zBtnX1, zBtnY1, zBtnX2, zBtnY2, 0.8f, 0.8f, 1.0f); // Very light blue when hovered
    } else {
        PrimitiveRenderer::drawRect(zBtnX1, zBtnY1, zBtnX2, zBtnY2, 0.0f, 0.0f, 1.0f); // Normal blue
    }
    PrimitiveRenderer::drawOutlineRect(zBtnX1, zBtnY1, zBtnX2, zBtnY2, 0.0f, 0.0f, 0.0f, 1.5f);

    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText("Z", zBtnX1 + buttonSize * 0.3f, zBtnY1 + buttonSize * 0.3f, GLUT_BITMAP_HELVETICA_10);

    // Store hover state for mouse click detection
    appState.axisHovered[0] = hoverX;
    appState.axisHovered[1] = hoverY;
    appState.axisHovered[2] = hoverZ;

    // Restore matrix state
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}




void drawTopBarUI(int width, int height) {
    float marginPx = 10.0f;
    float marginX = PrimitiveRenderer::pxToNDCx((int)marginPx, width);
    float topBarY2 = 1.0f;

    float blenderWpx = 120.0f;
    float blenderHpx = 48.0f;
    float blenderX1 = -1.0f + marginX;
    float blenderX2 = blenderX1 + PrimitiveRenderer::pxToNDCx((int)blenderWpx, width);
    float blenderH = PrimitiveRenderer::pxToNDCy((int)blenderHpx, height);
    float blenderY2 = topBarY2 - PrimitiveRenderer::pxToNDCy(8, height);
    float blenderY1 = blenderY2 - blenderH;

    PrimitiveRenderer::drawRect(blenderX1, blenderY1, blenderX2, blenderY2, 0.38f, 0.38f, 0.38f);
    PrimitiveRenderer::drawOutlineRect(blenderX1, blenderY1, blenderX2, blenderY2, 0.0f, 0.0f, 0.0f, 2.0f);

    glColor3f(1.0f, 0.0f, 0.0f);
    float textX = (blenderX1 + blenderX2) * 0.5f - 0.06f;
    float textY = (blenderY1 + blenderY2) * 0.5f - 0.01f;
    PrimitiveRenderer::drawText("Blender Lite", textX, textY, GLUT_BITMAP_HELVETICA_18);

    float groupX1 = blenderX2 + marginX;
    float groupX2 = groupX1 + PrimitiveRenderer::pxToNDCx((int)(140), width);

    float buttonHpx = 28.0f;
    float saveH = PrimitiveRenderer::pxToNDCy((int)buttonHpx, height);
    float gapV = PrimitiveRenderer::pxToNDCy(6, height);
    float save1Y2 = blenderY2;
    float save1Y1 = save1Y2 - saveH;
    float save2Y2 = save1Y1 - gapV;
    float save2Y1 = save2Y2 - saveH;

    bool hoverSave = PrimitiveRenderer::isInsideNDC(appState.mouseX, appState.mouseY, width, height, groupX1, save1Y1, groupX2, save1Y2);
    PrimitiveRenderer::drawRect(groupX1, save1Y1, groupX2, save1Y2, 0.32f + (hoverSave ? 0.08f : 0.0f), 0.32f + (hoverSave ? 0.08f : 0.0f), 0.32f + (hoverSave ? 0.08f : 0.0f));
    PrimitiveRenderer::drawOutlineRect(groupX1, save1Y1, groupX2, save1Y2, 0,0,0,1.5f);
    glColor3f(1,1,1);
    PrimitiveRenderer::drawText("Save", groupX1 + 0.02f, (save1Y1 + save1Y2) * 0.5f - 0.01f, GLUT_BITMAP_HELVETICA_18);

    bool hoverSaveAs = PrimitiveRenderer::isInsideNDC(appState.mouseX, appState.mouseY, width, height, groupX1, save2Y1, groupX2, save2Y2);
    PrimitiveRenderer::drawRect(groupX1, save2Y1, groupX2, save2Y2, 0.32f + (hoverSaveAs ? 0.08f : 0.0f), 0.32f + (hoverSaveAs ? 0.08f : 0.0f), 0.32f + (hoverSaveAs ? 0.08f : 0.0f));
    PrimitiveRenderer::drawOutlineRect(groupX1, save2Y1, groupX2, save2Y2, 0,0,0,1.5f);
    glColor3f(1,1,1);
    PrimitiveRenderer::drawText("Save as", groupX1 + 0.01f, (save2Y1 + save2Y2)*0.5f - 0.01f, GLUT_BITMAP_HELVETICA_18);

    float underY2 = save2Y1 - gapV - 0.0f;
    float underH = saveH;
    float underY1 = underY2 - underH;
    float halfW = (groupX2 - groupX1 - marginX) * 0.5f;
    float undoX1 = groupX1;
    float undoX2 = groupX1 + halfW - marginX*0.5f;
    float redoX1 = undoX2 + marginX*0.5f;
    float redoX2 = groupX2;
    bool hoverUndo = PrimitiveRenderer::isInsideNDC(appState.mouseX, appState.mouseY, width, height, undoX1, underY1, undoX2, underY2);
    bool hoverRedo = PrimitiveRenderer::isInsideNDC(appState.mouseX, appState.mouseY, width, height, redoX1, underY1, redoX2, underY2);

    PrimitiveRenderer::drawRect(undoX1, underY1, undoX2, underY2, 0.32f + (hoverUndo ? 0.08f : 0.0f), 0.32f + (hoverUndo ? 0.08f : 0.0f), 0.32f + (hoverUndo ? 0.08f : 0.0f));
    PrimitiveRenderer::drawOutlineRect(undoX1, underY1, undoX2, underY2, 0,0,0,1.0f);
    glColor3f(1,1,1);
    PrimitiveRenderer::drawText("Undo", undoX1 + 0.01f, (underY1 + underY2)*0.5f - 0.01f, GLUT_BITMAP_HELVETICA_18);

    PrimitiveRenderer::drawRect(redoX1, underY1, redoX2, underY2, 0.32f + (hoverRedo ? 0.08f : 0.0f), 0.32f + (hoverRedo ? 0.08f : 0.0f), 0.32f + (hoverRedo ? 0.08f : 0.0f));
    PrimitiveRenderer::drawOutlineRect(redoX1, underY1, redoX2, underY2, 0,0,0,1.0f);
    glColor3f(1,1,1);
    PrimitiveRenderer::drawText("Redo", redoX1 + 0.01f, (underY1 + underY2)*0.5f - 0.01f, GLUT_BITMAP_HELVETICA_18);

    float centerWpx = 300.0f;
    float centerX1 = -PrimitiveRenderer::pxToNDCx((int)(centerWpx/2), width);
    float centerX2 = PrimitiveRenderer::pxToNDCx((int)(centerWpx/2), width);

    float pnameHpx = 20.0f;
    float pnameH = PrimitiveRenderer::pxToNDCy((int)pnameHpx, height);
    float pnameY2 = blenderY2;
    float pnameY1 = pnameY2 - pnameH - PrimitiveRenderer::pxToNDCy(6, height);
    PrimitiveRenderer::drawRect(centerX1, pnameY1, centerX2, pnameY2, 0.9f, 0.9f, 0.9f);
    PrimitiveRenderer::drawOutlineRect(centerX1, pnameY1, centerX2, pnameY2, 0,0,0,1.0f);
    glColor3f(0.2f,0.2f,0.2f);
    PrimitiveRenderer::drawText("Project Name", centerX1 + 0.03f, (pnameY1 + pnameY2)*0.5f - 0.01f, GLUT_BITMAP_HELVETICA_18);

    float npWpx = 150.0f;
    float npX1 = -PrimitiveRenderer::pxToNDCx((int)(npWpx/2), width);
    float npX2 = PrimitiveRenderer::pxToNDCx((int)(npWpx/2), width);
    float npH = PrimitiveRenderer::pxToNDCy((int)buttonHpx, height);
    float npY2 = pnameY1 - PrimitiveRenderer::pxToNDCy(8, height);
    float npY1 = npY2 - npH;
    bool hoverNewProj = PrimitiveRenderer::isInsideNDC(appState.mouseX, appState.mouseY, width, height, npX1, npY1, npX2, npY2);
    PrimitiveRenderer::drawRect(npX1, npY1, npX2, npY2, 0.32f + (hoverNewProj ? 0.08f : 0.0f), 0.32f + (hoverNewProj ? 0.08f : 0.0f), 0.32f + (hoverNewProj ? 0.08f : 0.0f));
    PrimitiveRenderer::drawOutlineRect(npX1, npY1, npX2, npY2, 0,0,0,1.0f);
    glColor3f(1,1,1);
    PrimitiveRenderer::drawText("New Project", npX1 + 0.01f, (npY1 + npY2)*0.5f - 0.01f, GLUT_BITMAP_HELVETICA_18);

    float camSizePx = 48.0f;
    float camSize = PrimitiveRenderer::pxToNDCx((int)camSizePx, width);
    float camX2 = 1.0f - PrimitiveRenderer::pxToNDCx(50, width);
    float camX1 = camX2 - camSize;
    float camY2 = blenderY2;
    float camY1 = camY2 - PrimitiveRenderer::pxToNDCy((int)camSizePx, height);
    bool hoverCam = PrimitiveRenderer::isInsideNDC(appState.mouseX, appState.mouseY, width, height, camX1, camY1, camX2, camY2);
    float camExpandPx = hoverCam ? 6.0f : 0.0f;
    float camEx = PrimitiveRenderer::pxToNDCx((int)camExpandPx, width);

    PrimitiveRenderer::drawRect(camX1 - camEx, camY1 - camEx, camX2 + camEx, camY2 + camEx, 0.3f, 0.3f, 0.3f);
    PrimitiveRenderer::drawOutlineRect(camX1 - camEx, camY1 - camEx, camX2 + camEx, camY2 + camEx, 0,0,0,1.0f);

    float camCx = (camX1 + camX2) * 0.5f;
    float camCy = (camY1 + camY2) * 0.5f;
    PrimitiveRenderer::drawCircle(camCx, camCy, std::min(camSize, PrimitiveRenderer::pxToNDCy((int)camSizePx, height)) * 0.25f + (hoverCam ? camEx*0.5f : 0.0f), 30, 0.12f, 0.12f, 0.12f);

    glColor3f(1,1,1);
    float textLine1Y = camY1 - PrimitiveRenderer::pxToNDCy(14, height);
    float textLine2Y = textLine1Y - PrimitiveRenderer::pxToNDCy(22, height);
    PrimitiveRenderer::drawText("Take a", camCx - 0.05f, textLine1Y, GLUT_BITMAP_HELVETICA_12);
    PrimitiveRenderer::drawText("Screenshot", camCx - 0.09f, textLine2Y, GLUT_BITMAP_HELVETICA_12);

    float colorsPanelWpx = 220.0f;
    float colorsPanelHpx = 80.0f;
    float colorsPanelW = PrimitiveRenderer::pxToNDCx((int)colorsPanelWpx, width);
    float colorsPanelH = PrimitiveRenderer::pxToNDCy((int)colorsPanelHpx, height);

    float projectNameRight = centerX2;
    float cameraLeft = camX1;
    float colorsX1 = projectNameRight + PrimitiveRenderer::pxToNDCx(10, width);
    float colorsX2 = colorsX1 + colorsPanelW;

    if (colorsX2 > cameraLeft - PrimitiveRenderer::pxToNDCx(10, width)) {
        colorsX2 = cameraLeft - PrimitiveRenderer::pxToNDCx(10, width);
        colorsX1 = colorsX2 - colorsPanelW;
    }

    float colorsY2 = topBarY2 - PrimitiveRenderer::pxToNDCy(8, height);
    float colorsY1 = colorsY2 - colorsPanelH;

    PrimitiveRenderer::drawRect(colorsX1, colorsY1, colorsX2, colorsY2, 0.4f, 0.4f, 0.4f);
    PrimitiveRenderer::drawOutlineRect(colorsX1, colorsY1, colorsX2, colorsY2, 0, 0, 0, 2.0f);

    glColor3f(1, 1, 1);
    PrimitiveRenderer::drawText("Colors", colorsX1 + 0.02f, colorsY2 - PrimitiveRenderer::pxToNDCy(18, height), GLUT_BITMAP_HELVETICA_12);

    int numColors = 6;
    float swatchPx = 20.0f;
    float swatchGapPx = 5.0f;
    float swatchSize = PrimitiveRenderer::pxToNDCx((int)swatchPx, width);
    float swatchGap = PrimitiveRenderer::pxToNDCx((int)swatchGapPx, width);
    float startSwX = colorsX1 + PrimitiveRenderer::pxToNDCx(10, width);
    float swYTop = colorsY2 - PrimitiveRenderer::pxToNDCy(30, height);
    float swYBottom = swYTop - swatchSize;

    struct Color { float r, g, b; };
    Color colorArr[6] = {{0,0,0}, {1,1,1}, {1,0,0}, {0,1,0}, {0,0,1}, {1,0,1}};
    for (int i = 0; i < numColors; i++) {
        float sx1 = startSwX + i * (swatchSize + swatchGap);
        float sx2 = sx1 + swatchSize;
        bool hover = PrimitiveRenderer::isInsideNDC(appState.mouseX, appState.mouseY, width, height, sx1, swYBottom, sx2, swYTop);
        float expandPx = hover ? 2.0f : 0.0f;
        float ex = PrimitiveRenderer::pxToNDCx((int)expandPx, width);
        PrimitiveRenderer::drawRect(sx1 - ex, swYBottom - ex, sx2 + ex, swYTop + ex, colorArr[i].r, colorArr[i].g, colorArr[i].b);
        PrimitiveRenderer::drawOutlineRect(sx1 - ex, swYBottom - ex, sx2 + ex, swYTop + ex, 0, 0, 0, 1.0f);
    }

    float vBtnPx = 22.0f;
    float vW = PrimitiveRenderer::pxToNDCx((int)vBtnPx, width);
    float vX1 = startSwX;
    float vX2 = vX1 + vW;
    float vY2 = swYBottom - PrimitiveRenderer::pxToNDCy(5, height);
    float vY1 = vY2 - PrimitiveRenderer::pxToNDCy((int)vBtnPx, height);
    bool hoverV = PrimitiveRenderer::isInsideNDC(appState.mouseX, appState.mouseY, width, height, vX1, vY1, vX2, vY2);
    PrimitiveRenderer::drawRect(vX1, vY1, vX2, vY2, hoverV ? 0.6f : 0.25f, hoverV ? 0.6f : 0.25f, hoverV ? 0.6f : 0.25f);
    PrimitiveRenderer::drawOutlineRect(vX1, vY1, vX2, vY2, 0, 0, 0, 1.0f);
    glColor3f(1,1,1);
    PrimitiveRenderer::drawText("V", vX1 + 0.005f, (vY1 + vY2)*0.5f - 0.01f, GLUT_BITMAP_HELVETICA_12);

    float shadesWpx = 80.0f;
    float shadesHpx = 22.0f;
    float shadesW = PrimitiveRenderer::pxToNDCx((int)shadesWpx, width);
    float shadesH = PrimitiveRenderer::pxToNDCy((int)shadesHpx, height);
    float shadesX1 = vX2 + PrimitiveRenderer::pxToNDCx(5, width);
    float shadesX2 = shadesX1 + shadesW;
    float shadesY2 = vY2;
    float shadesY1 = vY2 - shadesH;
    PrimitiveRenderer::drawRect(shadesX1, shadesY1, shadesX2, shadesY2, 0.3f, 0.3f, 0.3f);
    PrimitiveRenderer::drawOutlineRect(shadesX1, shadesY1, shadesX2, shadesY2, 0, 0, 0, 1.0f);
    glColor3f(1,1,1);
    PrimitiveRenderer::drawText("Shades", shadesX1 + 0.01f, (shadesY1 + shadesY2)*0.5f - 0.01f, GLUT_BITMAP_HELVETICA_12);
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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    int argc = 0;
    char** argv = nullptr;
    glutInit(&argc, argv);
    glClearColor(0.12f, 0.12f, 0.15f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        if (width < Constants::MIN_WINDOW_WIDTH + appState.leftBarWidth + appState.rightBarWidth) {
            width = Constants::MIN_WINDOW_WIDTH + appState.leftBarWidth + appState.rightBarWidth;
        }

        float leftEdgeNDC = -1.0f + (2.0f * appState.leftBarWidth / width);
        float rightEdgeNDC = 1.0f - (2.0f * appState.rightBarWidth / width);
        float topEdgeNDC = 1.0f - (2.0f * Constants::TOP_BAR_HEIGHT / height);
        float handleNDC_W = 2.0f * Constants::HANDLE_PX / width;

        PrimitiveRenderer::drawRect(-1.0f, -1.0f, leftEdgeNDC, topEdgeNDC, 0.5f, 0.5f, 0.5f);
        PrimitiveRenderer::drawRect(rightEdgeNDC, -1.0f, 1.0f, topEdgeNDC, 0.5f, 0.5f, 0.5f);
        PrimitiveRenderer::drawRect(-1.0f, topEdgeNDC, 1.0f, 1.0f, 0.45f, 0.45f, 0.45f);
        PrimitiveRenderer::drawRect(leftEdgeNDC - handleNDC_W, -1.0f, leftEdgeNDC, topEdgeNDC, 0.7f, 0.7f, 0.7f);
        PrimitiveRenderer::drawRect(rightEdgeNDC, -1.0f, rightEdgeNDC + handleNDC_W, topEdgeNDC, 0.7f, 0.7f, 0.7f);

        drawTopBarUI(width, height);

        float leftPanelWidth = PrimitiveRenderer::pxToNDCx((int)(appState.leftBarWidth * 0.85f), width);
        float leftPanelHeight = PrimitiveRenderer::pxToNDCy(310, height);
        float leftPanelGap = PrimitiveRenderer::pxToNDCy(20, height);
        float leftVerticalOffset = PrimitiveRenderer::pxToNDCy(5, height);

        float texturesPanelY = -1.0f + appState.leftPanelTexturesScroll + leftVerticalOffset;
        Panels::drawTexturesPanel(-1.0f + PrimitiveRenderer::pxToNDCx(5, width), texturesPanelY,
            leftPanelWidth, leftPanelHeight, appState, width, height);

        float shapesPanelY = texturesPanelY + leftPanelHeight + leftPanelGap + appState.leftPanelShapesScroll;
        Panels::drawShapesPanel(-1.0f + PrimitiveRenderer::pxToNDCx(5, width), shapesPanelY,
            leftPanelWidth, leftPanelHeight, appState, width, height);

        float panelWidth = PrimitiveRenderer::pxToNDCx((int)(appState.rightBarWidth * 0.85f), width);
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

        float canvasX1 = leftEdgeNDC + PrimitiveRenderer::pxToNDCx(6, width);
        float canvasX2 = rightEdgeNDC - PrimitiveRenderer::pxToNDCx(6, width);
        float canvasY1 = -1.0f;
        float canvasY2 = topEdgeNDC - PrimitiveRenderer::pxToNDCy(6, height);
        PrimitiveRenderer::drawRect(canvasX1, canvasY1, canvasX2, canvasY2, 0.12f, 0.12f, 0.15f);
        PrimitiveRenderer::drawOutlineRect(canvasX1, canvasY1, canvasX2, canvasY2, 0.2f, 0.2f, 0.2f, 1.0f);

        // Draw 3D axes in the center of the canvas
        draw3DAxes(canvasX1, canvasY1, canvasX2, canvasY2, width, height);

        // Draw axis buttons attached to left sidebar's right edge
        drawAxisButtons(leftEdgeNDC, canvasY1, canvasY2, width, height);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
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

    float leftEdge = appState.leftBarWidth;
    float rightEdge = width - appState.rightBarWidth;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
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

        // Existing resizing logic
        if (xpos >= leftEdge - Constants::HANDLE_PX && xpos <= leftEdge)
            appState.resizingLeft = true;
        else if (xpos >= rightEdge && xpos <= rightEdge + Constants::HANDLE_PX)
            appState.resizingRight = true;
        appState.lastX = xpos;
        appState.lastY = ypos;
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        appState.resizingLeft = appState.resizingRight = false;
        appState.draggingAxis = false;
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
        return; // Skip resizing logic when dragging axes
    }

    if (!appState.resizingLeft && !appState.resizingRight) return;

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    double dx = xpos - appState.lastX;
    appState.lastX = xpos;
    appState.lastY = ypos;

    if (appState.resizingLeft) {
        appState.leftBarWidth = std::max(Constants::MIN_LEFT_BAR_WIDTH,
            std::min((float)(appState.leftBarWidth + dx),
            width - appState.rightBarWidth - Constants::MIN_WINDOW_WIDTH));
    }
    else if (appState.resizingRight) {
        appState.rightBarWidth = std::max(Constants::MIN_RIGHT_BAR_WIDTH,
            std::min((float)(appState.rightBarWidth - dx),
            width - appState.leftBarWidth - Constants::MIN_WINDOW_WIDTH));
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)window;
    (void)xoffset;

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    double mx, my;
    glfwGetCursorPos(window, &mx, &my);

    if (mx < appState.leftBarWidth) {
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