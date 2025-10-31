#include "../include/ui/Panels.hpp"
#include "../include/rendering/PrimitiveRenderer.hpp"
#include "../include/core/Constants.hpp"
#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cmath>
#include <sstream>
#include <iomanip>

// Add this new function to handle color button clicks
void handleColorButtonClick(int colorIndex, ApplicationState& state) {
    switch (colorIndex) {
        case 0: // Black
            state.currentColor[0] = Constants::COLOR_BLACK[0];
            state.currentColor[1] = Constants::COLOR_BLACK[1];
            state.currentColor[2] = Constants::COLOR_BLACK[2];
            break;
        case 1: // White
            state.currentColor[0] = Constants::COLOR_WHITE[0];
            state.currentColor[1] = Constants::COLOR_WHITE[1];
            state.currentColor[2] = Constants::COLOR_WHITE[2];
            break;
        case 2: // Red
            state.currentColor[0] = Constants::COLOR_RED[0];
            state.currentColor[1] = Constants::COLOR_RED[1];
            state.currentColor[2] = Constants::COLOR_RED[2];
            break;
        case 3: // Green
            state.currentColor[0] = Constants::COLOR_GREEN[0];
            state.currentColor[1] = Constants::COLOR_GREEN[1];
            state.currentColor[2] = Constants::COLOR_GREEN[2];
            break;
        case 4: // Blue
            state.currentColor[0] = Constants::COLOR_BLUE[0];
            state.currentColor[1] = Constants::COLOR_BLUE[1];
            state.currentColor[2] = Constants::COLOR_BLUE[2];
            break;
        case 5: // Magenta
            state.currentColor[0] = Constants::COLOR_MAGENTA[0];
            state.currentColor[1] = Constants::COLOR_MAGENTA[1];
            state.currentColor[2] = Constants::COLOR_MAGENTA[2];
            break;
    }

    // NEW: Apply color to current shape (remove texture)
    if (state.currentShape != ShapeType::NONE) {
        PrimitiveRenderer::applyColorToShape(state.currentShape, state);
        std::cout << "Color changed to button " << colorIndex << " and applied to current shape" << std::endl;
    } else {
        std::cout << "Color changed to button " << colorIndex << " (no shape selected)" << std::endl;
    }
}

void Panels::drawTopBarUI(int width, int height, ApplicationState& appState) {
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

    // Define colors for the buttons
    struct Color { float r, g, b; };
    Color colorArr[6] = {{0,0,0}, {1,1,1}, {1,0,0}, {0,1,0}, {0,0,1}, {1,0,1}};

    // Track hover states for color buttons
    bool colorHovered[6] = {false};

    for (int i = 0; i < numColors; i++) {
        float sx1 = startSwX + i * (swatchSize + swatchGap);
        float sx2 = sx1 + swatchSize;
        bool hover = PrimitiveRenderer::isInsideNDC(appState.mouseX, appState.mouseY, width, height, sx1, swYBottom, sx2, swYTop);
        colorHovered[i] = hover;
        float expandPx = hover ? 2.0f : 0.0f;
        float ex = PrimitiveRenderer::pxToNDCx((int)expandPx, width);
        PrimitiveRenderer::drawRect(sx1 - ex, swYBottom - ex, sx2 + ex, swYTop + ex, colorArr[i].r, colorArr[i].g, colorArr[i].b);
        PrimitiveRenderer::drawOutlineRect(sx1 - ex, swYBottom - ex, sx2 + ex, swYTop + ex, 0, 0, 0, 1.0f);

        // Handle color button clicks
        if (hover && appState.mouseClicked) {
            handleColorButtonClick(i, appState);
        }
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

void Panels::drawTransformPanel(float panelX1, float panelY1, float panelWidth, float panelHeight,
                               const std::string& title, float values[3],
                               ApplicationState& state, int width, int height) {
    PrimitiveRenderer::drawRect(panelX1, panelY1, panelX1 + panelWidth, panelY1 + panelHeight, 0.4f, 0.4f, 0.4f);
    PrimitiveRenderer::drawOutlineRect(panelX1, panelY1, panelX1 + panelWidth, panelY1 + panelHeight, 0.0f, 0.0f, 0.0f, 2.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText(title, panelX1 + PrimitiveRenderer::pxToNDCx(10, width), panelY1 + panelHeight - PrimitiveRenderer::pxToNDCy(25, height), GLUT_BITMAP_HELVETICA_18);
    PrimitiveRenderer::drawText("Vector", panelX1 + PrimitiveRenderer::pxToNDCx(15, width), panelY1 + panelHeight - PrimitiveRenderer::pxToNDCy(45, height), GLUT_BITMAP_HELVETICA_12);

    float inputBoxHeight = PrimitiveRenderer::pxToNDCy(25, height);
    float inputBoxWidth = panelWidth - PrimitiveRenderer::pxToNDCx(30, width);
    float inputY = panelY1 + panelHeight - PrimitiveRenderer::pxToNDCy(70, height);

    // Determine panel type for input field identification
    int panelType = -1;
    if (title == "Rotation") panelType = 0;
    else if (title == "Scaling") panelType = 1;
    else if (title == "Translate") panelType = 2;

    // X input box
    float xBoxX1 = panelX1 + PrimitiveRenderer::pxToNDCx(15, width);
    float xBoxX2 = xBoxX1 + inputBoxWidth;
    float xBoxY1 = inputY - inputBoxHeight;
    float xBoxY2 = inputY;

    bool isXActive = (state.activeInputField.panelType == panelType && state.activeInputField.axis == 0);
    bool hoverX = PrimitiveRenderer::isInsideNDC(state.mouseX, state.mouseY, width, height, xBoxX1, xBoxY1, xBoxX2, xBoxY2);

    // Draw input box with different color if active or hovered
    if (isXActive) {
        PrimitiveRenderer::drawRect(xBoxX1, xBoxY1, xBoxX2, xBoxY2, 0.7f, 0.7f, 0.9f);
    } else if (hoverX) {
        PrimitiveRenderer::drawRect(xBoxX1, xBoxY1, xBoxX2, xBoxY2, 0.6f, 0.6f, 0.6f);
    } else {
        PrimitiveRenderer::drawRect(xBoxX1, xBoxY1, xBoxX2, xBoxY2, 0.9f, 0.9f, 0.9f);
    }
    PrimitiveRenderer::drawOutlineRect(xBoxX1, xBoxY1, xBoxX2, xBoxY2, 0.0f, 0.0f, 0.0f, 1.0f);

    glColor3f(0.2f, 0.2f, 0.2f);
    std::string xText;
    if (isXActive) {
        xText = "X: " + state.activeInputField.text;
    } else {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(3) << values[0];
        xText = "X: " + stream.str();
    }
    PrimitiveRenderer::drawText(xText, xBoxX1 + PrimitiveRenderer::pxToNDCx(5, width), inputY - PrimitiveRenderer::pxToNDCy(15, height), GLUT_BITMAP_HELVETICA_12);

    // Y input box
    inputY -= inputBoxHeight + PrimitiveRenderer::pxToNDCy(2, height);
    float yBoxX1 = panelX1 + PrimitiveRenderer::pxToNDCx(15, width);
    float yBoxX2 = yBoxX1 + inputBoxWidth;
    float yBoxY1 = inputY - inputBoxHeight;
    float yBoxY2 = inputY;

    bool isYActive = (state.activeInputField.panelType == panelType && state.activeInputField.axis == 1);
    bool hoverY = PrimitiveRenderer::isInsideNDC(state.mouseX, state.mouseY, width, height, yBoxX1, yBoxY1, yBoxX2, yBoxY2);

    if (isYActive) {
        PrimitiveRenderer::drawRect(yBoxX1, yBoxY1, yBoxX2, yBoxY2, 0.7f, 0.7f, 0.9f);
    } else if (hoverY) {
        PrimitiveRenderer::drawRect(yBoxX1, yBoxY1, yBoxX2, yBoxY2, 0.6f, 0.6f, 0.6f);
    } else {
        PrimitiveRenderer::drawRect(yBoxX1, yBoxY1, yBoxX2, yBoxY2, 0.9f, 0.9f, 0.9f);
    }
    PrimitiveRenderer::drawOutlineRect(yBoxX1, yBoxY1, yBoxX2, yBoxY2, 0.0f, 0.0f, 0.0f, 1.0f);

    glColor3f(0.2f, 0.2f, 0.2f);
    std::string yText;
    if (isYActive) {
        yText = "Y: " + state.activeInputField.text;
    } else {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(3) << values[1];
        yText = "Y: " + stream.str();
    }
    PrimitiveRenderer::drawText(yText, yBoxX1 + PrimitiveRenderer::pxToNDCx(5, width), inputY - PrimitiveRenderer::pxToNDCy(15, height), GLUT_BITMAP_HELVETICA_12);

    // Z input box
    inputY -= inputBoxHeight + PrimitiveRenderer::pxToNDCy(2, height);
    float zBoxX1 = panelX1 + PrimitiveRenderer::pxToNDCx(15, width);
    float zBoxX2 = zBoxX1 + inputBoxWidth;
    float zBoxY1 = inputY - inputBoxHeight;
    float zBoxY2 = inputY;

    bool isZActive = (state.activeInputField.panelType == panelType && state.activeInputField.axis == 2);
    bool hoverZ = PrimitiveRenderer::isInsideNDC(state.mouseX, state.mouseY, width, height, zBoxX1, zBoxY1, zBoxX2, zBoxY2);

    if (isZActive) {
        PrimitiveRenderer::drawRect(zBoxX1, zBoxY1, zBoxX2, zBoxY2, 0.7f, 0.7f, 0.9f);
    } else if (hoverZ) {
        PrimitiveRenderer::drawRect(zBoxX1, zBoxY1, zBoxX2, zBoxY2, 0.6f, 0.6f, 0.6f);
    } else {
        PrimitiveRenderer::drawRect(zBoxX1, zBoxY1, zBoxX2, zBoxY2, 0.9f, 0.9f, 0.9f);
    }
    PrimitiveRenderer::drawOutlineRect(zBoxX1, zBoxY1, zBoxX2, zBoxY2, 0.0f, 0.0f, 0.0f, 1.0f);

    glColor3f(0.2f, 0.2f, 0.2f);
    std::string zText;
    if (isZActive) {
        zText = "Z: " + state.activeInputField.text;
    } else {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(3) << values[2];
        zText = "Z: " + stream.str();
    }
    PrimitiveRenderer::drawText(zText, zBoxX1 + PrimitiveRenderer::pxToNDCx(5, width), inputY - PrimitiveRenderer::pxToNDCy(15, height), GLUT_BITMAP_HELVETICA_12);

    // Store original values for cancellation
    static float originalValues[3] = {0.0f, 0.0f, 0.0f};
    static bool hasOriginalValues = false;

    // Handle input box clicks
    if (state.mouseClicked) {
        if (hoverX && !isXActive) {
            // Store original value before activating
            originalValues[0] = values[0];
            originalValues[1] = values[1];
            originalValues[2] = values[2];
            hasOriginalValues = true;

            state.activeInputField.panelType = panelType;
            state.activeInputField.axis = 0;
            state.activeInputField.active = true;
            state.activeInputField.text = ""; // Start with empty text
            std::cout << "Editing " << title << " X value\n";
        } else if (hoverY && !isYActive) {
            // Store original value before activating
            originalValues[0] = values[0];
            originalValues[1] = values[1];
            originalValues[2] = values[2];
            hasOriginalValues = true;

            state.activeInputField.panelType = panelType;
            state.activeInputField.axis = 1;
            state.activeInputField.active = true;
            state.activeInputField.text = ""; // Start with empty text
            std::cout << "Editing " << title << " Y value\n";
        } else if (hoverZ && !isZActive) {
            // Store original value before activating
            originalValues[0] = values[0];
            originalValues[1] = values[1];
            originalValues[2] = values[2];
            hasOriginalValues = true;

            state.activeInputField.panelType = panelType;
            state.activeInputField.axis = 2;
            state.activeInputField.active = true;
            state.activeInputField.text = ""; // Start with empty text
            std::cout << "Editing " << title << " Z value\n";
        }
    }

    float resetBtnY = inputY - inputBoxHeight - PrimitiveRenderer::pxToNDCy(10, height);
    float resetBtnWidth = PrimitiveRenderer::pxToNDCx(60, width);
    float resetBtnHeight = PrimitiveRenderer::pxToNDCy(25, height);
    bool hoverReset = PrimitiveRenderer::isInsideNDC(state.mouseX, state.mouseY, width, height,
                                                   panelX1 + PrimitiveRenderer::pxToNDCx(15, width), resetBtnY - resetBtnHeight,
                                                   panelX1 + PrimitiveRenderer::pxToNDCx(15, width) + resetBtnWidth, resetBtnY);

    PrimitiveRenderer::drawRect(panelX1 + PrimitiveRenderer::pxToNDCx(15, width), resetBtnY - resetBtnHeight,
                               panelX1 + PrimitiveRenderer::pxToNDCx(15, width) + resetBtnWidth, resetBtnY,
                               0.32f + (hoverReset ? 0.08f : 0.0f), 0.32f + (hoverReset ? 0.08f : 0.0f), 0.32f + (hoverReset ? 0.08f : 0.0f));
    PrimitiveRenderer::drawOutlineRect(panelX1 + PrimitiveRenderer::pxToNDCx(15, width), resetBtnY - resetBtnHeight,
                                     panelX1 + PrimitiveRenderer::pxToNDCx(15, width) + resetBtnWidth, resetBtnY, 0,0,0,1.0f);

    glColor3f(1,1,1);
    PrimitiveRenderer::drawText("Reset", panelX1 + PrimitiveRenderer::pxToNDCx(25, width), resetBtnY - PrimitiveRenderer::pxToNDCy(15, height), GLUT_BITMAP_HELVETICA_12);

    // Handle reset button click for all panels
    if (hoverReset && state.mouseClicked) {
        if (title == "Translate") {
            // Reset translation to origin - NO LIMITS
            state.translate[0] = 0.0f;
            state.translate[1] = 0.0f;
            state.translate[2] = 0.0f;
            std::cout << "Translation reset to origin\n";
        } else if (title == "Scaling") {
            state.scale[0] = 1.0f;
            state.scale[1] = 1.0f;
            state.scale[2] = 1.0f;
            std::cout << "Scaling reset to default values\n";
        } else if (title == "Rotation") {
            state.rotate[0] = 0.0f;
            state.rotate[1] = 0.0f;
            state.rotate[2] = 0.0f;
            std::cout << "Rotation reset to default values\n";
        }
    }
}

void Panels::drawShapesPanel(float panelX1, float panelY1, float panelWidth, float panelHeight,
                            ApplicationState& state, int width, int height) {
    int panelXPx = (int)((panelX1 + 1.0f) * 0.5f * width);
    int panelYPx = (int)((1.0f - (panelY1 + panelHeight)) * 0.5f * height);
    int panelWidthPx = (int)(panelWidth * 0.5f * width);
    int panelHeightPx = (int)(panelHeight * 0.5f * height);

    glEnable(GL_SCISSOR_TEST);
    PrimitiveRenderer::setScissor(panelXPx, panelYPx, panelWidthPx, panelHeightPx, height);

    PrimitiveRenderer::drawRect(panelX1, panelY1, panelX1 + panelWidth, panelY1 + panelHeight, 0.4f, 0.4f, 0.4f);
    PrimitiveRenderer::drawOutlineRect(panelX1, panelY1, panelX1 + panelWidth, panelY1 + panelHeight, 0.0f, 0.0f, 0.0f, 2.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText("Shapes", panelX1 + PrimitiveRenderer::pxToNDCx(10, width), panelY1 + panelHeight - PrimitiveRenderer::pxToNDCy(25, height), GLUT_BITMAP_HELVETICA_18);

    float buttonWidth = (panelWidth - PrimitiveRenderer::pxToNDCx(40, width)) / 2.0f;
    float buttonHeight = PrimitiveRenderer::pxToNDCy(75, height);
    float buttonGap = PrimitiveRenderer::pxToNDCx(8, width);

    float rowGap = PrimitiveRenderer::pxToNDCy(10, height);
    float contentHeight = (buttonHeight * 3) + (rowGap * 2);
    float maxScroll = std::max(0.0f, contentHeight - panelHeight);

    float scrollOffset = std::max(0.0f, std::min(maxScroll, -state.leftPanelShapesScroll));

    float row1Y = panelY1 + panelHeight - PrimitiveRenderer::pxToNDCy(50, height) + scrollOffset;

    // Cube button
    float cubeX1 = panelX1 + PrimitiveRenderer::pxToNDCx(10, width);
    float cubeX2 = cubeX1 + buttonWidth;
    bool hoverCube = PrimitiveRenderer::isInsideNDC(state.mouseX, state.mouseY, width, height, cubeX1, row1Y - buttonHeight, cubeX2, row1Y);
    bool isCubeSelected = (state.currentShape == ShapeType::CUBE);

    PrimitiveRenderer::drawRect(cubeX1, row1Y - buttonHeight, cubeX2, row1Y,
                              isCubeSelected ? 0.5f : (0.3f + (hoverCube ? 0.1f : 0.0f)),
                              isCubeSelected ? 0.5f : (0.3f + (hoverCube ? 0.1f : 0.0f)),
                              isCubeSelected ? 0.5f : (0.3f + (hoverCube ? 0.1f : 0.0f)));
    PrimitiveRenderer::drawOutlineRect(cubeX1, row1Y - buttonHeight, cubeX2, row1Y, 0.0f, 0.0f, 0.0f, 1.5f);

    // Draw 3D cube symbol
    float symbolSize = 0.03f;
    float symbolX = cubeX1 + buttonWidth * 0.5f;
    float symbolY = row1Y - buttonHeight * 0.5f;

    glPushMatrix();
    glTranslatef(symbolX, symbolY, 0.0f);
    glScalef(symbolSize, symbolSize, symbolSize);

    // Front face (light gray)
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_QUADS);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glEnd();

    // Right face (medium gray)
    glColor3f(0.6f, 0.6f, 0.6f);
    glBegin(GL_QUADS);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glEnd();

    // Top face (dark gray)
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_QUADS);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glEnd();

    glPopMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText("Cube", cubeX1 + PrimitiveRenderer::pxToNDCx(20, width), row1Y - PrimitiveRenderer::pxToNDCy(35, height), GLUT_BITMAP_HELVETICA_12);

    // Sphere button
    float sphereX1 = cubeX2 + buttonGap;
    float sphereX2 = sphereX1 + buttonWidth;
    bool hoverSphere = PrimitiveRenderer::isInsideNDC(state.mouseX, state.mouseY, width, height, sphereX1, row1Y - buttonHeight, sphereX2, row1Y);
    bool isSphereSelected = (state.currentShape == ShapeType::SPHERE);

    PrimitiveRenderer::drawRect(sphereX1, row1Y - buttonHeight, sphereX2, row1Y,
                              isSphereSelected ? 0.5f : (0.3f + (hoverSphere ? 0.1f : 0.0f)),
                              isSphereSelected ? 0.5f : (0.3f + (hoverSphere ? 0.1f : 0.0f)),
                              isSphereSelected ? 0.5f : (0.3f + (hoverSphere ? 0.1f : 0.0f)));
    PrimitiveRenderer::drawOutlineRect(sphereX1, row1Y - buttonHeight, sphereX2, row1Y, 0.0f, 0.0f, 0.0f, 1.5f);

    // Draw 3D sphere symbol
    symbolX = sphereX1 + buttonWidth * 0.5f;
    symbolY = row1Y - buttonHeight * 0.5f;

    glPushMatrix();
    glTranslatef(symbolX, symbolY, 0.0f);
    glScalef(symbolSize, symbolSize, symbolSize);

    int slices = 8;
    int stacks = 6;
    for (int i = 0; i <= stacks; ++i) {
        float phi = Constants::PI * i / stacks;
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float theta = 2.0f * Constants::PI * j / slices;
            float x = sin(phi) * cos(theta);
            float y = cos(phi);
            float z = sin(phi) * sin(theta);

            // Vary color based on position for 3D effect
            float shade = 0.5f + y * 0.3f;
            glColor3f(shade, shade, shade);
            glVertex3f(x, y, z);

            x = sin(phi + Constants::PI / stacks) * cos(theta);
            y = cos(phi + Constants::PI / stacks);
            z = sin(phi + Constants::PI / stacks) * sin(theta);

            shade = 0.5f + y * 0.3f;
            glColor3f(shade, shade, shade);
            glVertex3f(x, y, z);
        }
        glEnd();
    }
    glPopMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText("Sphere", sphereX1 + PrimitiveRenderer::pxToNDCx(20, width), row1Y - PrimitiveRenderer::pxToNDCy(35, height), GLUT_BITMAP_HELVETICA_12);

    float row2Y = row1Y - buttonHeight - rowGap;

    // Cone button
    float coneX1 = panelX1 + PrimitiveRenderer::pxToNDCx(10, width);
    float coneX2 = coneX1 + buttonWidth;
    bool hoverCone = PrimitiveRenderer::isInsideNDC(state.mouseX, state.mouseY, width, height, coneX1, row2Y - buttonHeight, coneX2, row2Y);
    bool isConeSelected = (state.currentShape == ShapeType::CONE);

    PrimitiveRenderer::drawRect(coneX1, row2Y - buttonHeight, coneX2, row2Y,
                              isConeSelected ? 0.5f : (0.3f + (hoverCone ? 0.1f : 0.0f)),
                              isConeSelected ? 0.5f : (0.3f + (hoverCone ? 0.1f : 0.0f)),
                              isConeSelected ? 0.5f : (0.3f + (hoverCone ? 0.1f : 0.0f)));
    PrimitiveRenderer::drawOutlineRect(coneX1, row2Y - buttonHeight, coneX2, row2Y, 0.0f, 0.0f, 0.0f, 1.5f);

    // Draw 3D cone symbol
    symbolX = coneX1 + buttonWidth * 0.5f;
    symbolY = row2Y - buttonHeight * 0.5f;

    glPushMatrix();
    glTranslatef(symbolX, symbolY, 0.0f);
    glScalef(symbolSize, symbolSize, symbolSize);

    // Base (dark gray)
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, -1.0f, 0.0f);
    for (int i = 0; i <= 16; ++i) {
        float theta = 2.0f * Constants::PI * i / 16;
        float x = cos(theta);
        float z = sin(theta);
        glVertex3f(x, -1.0f, z);
    }
    glEnd();

    // Side (gradient from dark to light)
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 1.0f, 0.0f);
    for (int i = 0; i <= 16; ++i) {
        float theta = 2.0f * Constants::PI * i / 16;
        float x = cos(theta);
        float z = sin(theta);
        float shade = 0.6f - fabs(sin(theta)) * 0.2f;
        glColor3f(shade, shade, shade);
        glVertex3f(x, -1.0f, z);
    }
    glEnd();
    glPopMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText("Cone", coneX1 + PrimitiveRenderer::pxToNDCx(25, width), row2Y - PrimitiveRenderer::pxToNDCy(35, height), GLUT_BITMAP_HELVETICA_12);

    // Cylinder button
    float cylinderX1 = coneX2 + buttonGap;
    float cylinderX2 = cylinderX1 + buttonWidth;
    bool hoverCylinder = PrimitiveRenderer::isInsideNDC(state.mouseX, state.mouseY, width, height, cylinderX1, row2Y - buttonHeight, cylinderX2, row2Y);
    bool isCylinderSelected = (state.currentShape == ShapeType::CYLINDER);

    PrimitiveRenderer::drawRect(cylinderX1, row2Y - buttonHeight, cylinderX2, row2Y,
                              isCylinderSelected ? 0.5f : (0.3f + (hoverCylinder ? 0.1f : 0.0f)),
                              isCylinderSelected ? 0.5f : (0.3f + (hoverCylinder ? 0.1f : 0.0f)),
                              isCylinderSelected ? 0.5f : (0.3f + (hoverCylinder ? 0.1f : 0.0f)));
    PrimitiveRenderer::drawOutlineRect(cylinderX1, row2Y - buttonHeight, cylinderX2, row2Y, 0.0f, 0.0f, 0.0f, 1.5f);

    // Draw 3D cylinder symbol
    symbolX = cylinderX1 + buttonWidth * 0.5f;
    symbolY = row2Y - buttonHeight * 0.5f;

    glPushMatrix();
    glTranslatef(symbolX, symbolY, 0.0f);
    glScalef(symbolSize * 0.8f, symbolSize * 0.8f, symbolSize * 0.8f);

    // Top (light gray)
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 1.0f, 0.0f);
    for (int i = 0; i <= 16; ++i) {
        float theta = 2.0f * Constants::PI * i / 16;
        glVertex3f(cos(theta), 1.0f, sin(theta));
    }
    glEnd();

    // Bottom (dark gray)
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, -1.0f, 0.0f);
    for (int i = 0; i <= 16; ++i) {
        float theta = 2.0f * Constants::PI * i / 16;
        glVertex3f(cos(theta), -1.0f, sin(theta));
    }
    glEnd();

    // Side (medium gray)
    glColor3f(0.6f, 0.6f, 0.6f);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= 16; ++i) {
        float theta = 2.0f * Constants::PI * i / 16;
        float x = cos(theta);
        float z = sin(theta);
        glVertex3f(x, 1.0f, z);
        glVertex3f(x, -1.0f, z);
    }
    glEnd();
    glPopMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText("Cylinder", cylinderX1 + PrimitiveRenderer::pxToNDCx(15, width), row2Y - PrimitiveRenderer::pxToNDCy(35, height), GLUT_BITMAP_HELVETICA_12);

    float row3Y = row2Y - buttonHeight - rowGap;

    // Torus button
    float torusX1 = panelX1 + PrimitiveRenderer::pxToNDCx(10, width);
    float torusX2 = torusX1 + buttonWidth;
    bool hoverTorus = PrimitiveRenderer::isInsideNDC(state.mouseX, state.mouseY, width, height, torusX1, row3Y - buttonHeight, torusX2, row3Y);
    bool isTorusSelected = (state.currentShape == ShapeType::TORUS);

    PrimitiveRenderer::drawRect(torusX1, row3Y - buttonHeight, torusX2, row3Y,
                              isTorusSelected ? 0.5f : (0.3f + (hoverTorus ? 0.1f : 0.0f)),
                              isTorusSelected ? 0.5f : (0.3f + (hoverTorus ? 0.1f : 0.0f)),
                              isTorusSelected ? 0.5f : (0.3f + (hoverTorus ? 0.1f : 0.0f)));
    PrimitiveRenderer::drawOutlineRect(torusX1, row3Y - buttonHeight, torusX2, row3Y, 0.0f, 0.0f, 0.0f, 1.5f);

    // Draw 3D torus symbol
    symbolX = torusX1 + buttonWidth * 0.5f;
    symbolY = row3Y - buttonHeight * 0.5f;

    glPushMatrix();
    glTranslatef(symbolX, symbolY, 0.0f);
    glScalef(symbolSize, symbolSize, symbolSize);

    float majorRadius = 1.0f;
    float minorRadius = 0.3f;
    int majorSlices = 12;
    int minorSlices = 8;

    for (int i = 0; i < majorSlices; ++i) {
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= minorSlices; ++j) {
            for (int k = 0; k <= 1; ++k) {
                float phi = 2.0f * Constants::PI * (i + k) / majorSlices;
                float theta = 2.0f * Constants::PI * j / minorSlices;

                float x = (majorRadius + minorRadius * cos(theta)) * cos(phi);
                float y = minorRadius * sin(theta);
                float z = (majorRadius + minorRadius * cos(theta)) * sin(phi);

                // Vary color based on position
                float shade = 0.5f + cos(theta) * 0.3f;
                glColor3f(shade, shade, shade);
                glVertex3f(x, y, z);
            }
        }
        glEnd();
    }
    glPopMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText("Torus", torusX1 + PrimitiveRenderer::pxToNDCx(25, width), row3Y - PrimitiveRenderer::pxToNDCy(35, height), GLUT_BITMAP_HELVETICA_12);

    // Pyramid button
    float pyramidX1 = torusX2 + buttonGap;
    float pyramidX2 = pyramidX1 + buttonWidth;
    bool hoverPyramid = PrimitiveRenderer::isInsideNDC(state.mouseX, state.mouseY, width, height, pyramidX1, row3Y - buttonHeight, pyramidX2, row3Y);
    bool isPyramidSelected = (state.currentShape == ShapeType::PYRAMID);

    PrimitiveRenderer::drawRect(pyramidX1, row3Y - buttonHeight, pyramidX2, row3Y,
                              isPyramidSelected ? 0.5f : (0.3f + (hoverPyramid ? 0.1f : 0.0f)),
                              isPyramidSelected ? 0.5f : (0.3f + (hoverPyramid ? 0.1f : 0.0f)),
                              isPyramidSelected ? 0.5f : (0.3f + (hoverPyramid ? 0.1f : 0.0f)));
    PrimitiveRenderer::drawOutlineRect(pyramidX1, row3Y - buttonHeight, pyramidX2, row3Y, 0.0f, 0.0f, 0.0f, 1.5f);

    // Draw 3D pyramid symbol
    symbolX = pyramidX1 + buttonWidth * 0.5f;
    symbolY = row3Y - buttonHeight * 0.5f;

    glPushMatrix();
    glTranslatef(symbolX, symbolY, 0.0f);
    glScalef(symbolSize, symbolSize, symbolSize);

    // Base (dark gray)
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_QUADS);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glEnd();

    // Front face (medium gray)
    glColor3f(0.6f, 0.6f, 0.6f);
    glBegin(GL_TRIANGLES);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();

    // Right face (light gray)
    glColor3f(0.7f, 0.7f, 0.7f);
    glBegin(GL_TRIANGLES);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();

    // Back face (medium-dark gray)
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_TRIANGLES);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();

    // Left face (medium-light gray)
    glColor3f(0.65f, 0.65f, 0.65f);
    glBegin(GL_TRIANGLES);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();
    glPopMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText("Pyramid", pyramidX1 + PrimitiveRenderer::pxToNDCx(20, width), row3Y - PrimitiveRenderer::pxToNDCy(35, height), GLUT_BITMAP_HELVETICA_12);

    glDisable(GL_SCISSOR_TEST);

    if (maxScroll > 0) {
        float scrollBarWidth = PrimitiveRenderer::pxToNDCx(8, width);
        float scrollBarX = panelX1 + panelWidth - scrollBarWidth - PrimitiveRenderer::pxToNDCx(5, width);
        float scrollBarHeight = panelHeight - PrimitiveRenderer::pxToNDCy(15, height);
        float scrollThumbHeight = scrollBarHeight * (panelHeight / contentHeight);
        float scrollProgress = state.leftPanelShapesScroll / maxScroll;
        float scrollThumbY = panelY1 + panelHeight - PrimitiveRenderer::pxToNDCy(10, height) - scrollThumbHeight - (scrollProgress * (scrollBarHeight - scrollThumbHeight));

        PrimitiveRenderer::drawRect(scrollBarX, panelY1 + PrimitiveRenderer::pxToNDCy(10, height), scrollBarX + scrollBarWidth, panelY1 + panelHeight - PrimitiveRenderer::pxToNDCy(10, height), 0.3f, 0.3f, 0.3f);
        PrimitiveRenderer::drawRect(scrollBarX, scrollThumbY, scrollBarX + scrollBarWidth, scrollThumbY + scrollThumbHeight, 0.6f, 0.6f, 0.6f);
    }
}

void Panels::drawTexturesPanel(float panelX1, float panelY1, float panelWidth, float panelHeight,
                              ApplicationState& state, int width, int height) {
    // Enable scissor test for this panel
    int panelXPx = (int)((panelX1 + 1.0f) * 0.5f * width);
    int panelYPx = (int)((1.0f - (panelY1 + panelHeight)) * 0.5f * height);
    int panelWidthPx = (int)(panelWidth * 0.5f * width);
    int panelHeightPx = (int)(panelHeight * 0.5f * height);

    glEnable(GL_SCISSOR_TEST);
    PrimitiveRenderer::setScissor(panelXPx, panelYPx, panelWidthPx, panelHeightPx, height);

    PrimitiveRenderer::drawRect(panelX1, panelY1, panelX1 + panelWidth, panelY1 + panelHeight, 0.4f, 0.4f, 0.4f);
    PrimitiveRenderer::drawOutlineRect(panelX1, panelY1, panelX1 + panelWidth, panelY1 + panelHeight, 0.0f, 0.0f, 0.0f, 2.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText("Textures", panelX1 + PrimitiveRenderer::pxToNDCx(10, width), panelY1 + panelHeight - PrimitiveRenderer::pxToNDCy(25, height), GLUT_BITMAP_HELVETICA_18);

    // Use the same layout as shapes panel for consistency
    float buttonWidth = (panelWidth - PrimitiveRenderer::pxToNDCx(40, width)) / 2.0f;
    float buttonHeight = PrimitiveRenderer::pxToNDCy(75, height);
    float buttonGap = PrimitiveRenderer::pxToNDCx(8, width);
    float rowGap = PrimitiveRenderer::pxToNDCy(10, height);

    float contentHeight = (buttonHeight * 3) + (rowGap * 2);
    float maxScroll = std::max(0.0f, contentHeight - panelHeight);
    float scrollOffset = std::max(0.0f, std::min(maxScroll, -state.leftPanelTexturesScroll));

    float row1Y = panelY1 + panelHeight - PrimitiveRenderer::pxToNDCy(50, height) + scrollOffset;

    // Texture buttons - 2 columns, 3 rows
    for (int i = 0; i < 6; i++) {
        int row = i / 2;
        int col = i % 2;

        float buttonX1 = panelX1 + PrimitiveRenderer::pxToNDCx(10, width) + col * (buttonWidth + buttonGap);
        float buttonX2 = buttonX1 + buttonWidth;
        float buttonY1 = row1Y - buttonHeight - row * (buttonHeight + rowGap);
        float buttonY2 = buttonY1 + buttonHeight;

        bool hover = PrimitiveRenderer::isInsideNDC(state.mouseX, state.mouseY, width, height, buttonX1, buttonY1, buttonX2, buttonY2);

        // Draw button background
        PrimitiveRenderer::drawRect(buttonX1, buttonY1, buttonX2, buttonY2,
                                  0.3f + (hover ? 0.1f : 0.0f),
                                  0.3f + (hover ? 0.1f : 0.0f),
                                  0.3f + (hover ? 0.1f : 0.0f));
        PrimitiveRenderer::drawOutlineRect(buttonX1, buttonY1, buttonX2, buttonY2, 0.0f, 0.0f, 0.0f, 1.5f);

        // Draw texture covering most of the button - INCREASED MARGIN
        if (i < PrimitiveRenderer::textureIDs.size()) {
            float textureMargin = PrimitiveRenderer::pxToNDCx(15, width); // Increased margin
            float textureX1 = buttonX1 + textureMargin;
            float textureX2 = buttonX2 - textureMargin;
            float textureY1 = buttonY1 + textureMargin;
            float textureY2 = buttonY2 - textureMargin;

            // Save current matrix state
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();

            // Draw the textured rectangle
            PrimitiveRenderer::drawTexturedRect(textureX1, textureY1, textureX2, textureY2, PrimitiveRenderer::textureIDs[i]);

            // Restore matrix state
            glPopMatrix();
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
        } else {
            // Draw placeholder if no texture
            PrimitiveRenderer::drawRect(buttonX1, buttonY1, buttonX2, buttonY2, 0.8f, 0.2f, 0.2f);
        }

        // Handle texture selection AND application to current shape
        if (hover && state.mouseClicked) {
            state.selectedTexture = i;
            std::cout << "Selected texture " << i << std::endl;

            // NEW: Apply texture to current shape if one is selected
            if (state.currentShape != ShapeType::NONE) {
                PrimitiveRenderer::applyTextureToShape(state.currentShape, i, state);
                std::cout << "Applied texture " << i << " to current shape" << std::endl;
            } else {
                std::cout << "No shape selected to apply texture to" << std::endl;
            }
        }

        // Draw selection indicator
        if (state.selectedTexture == i) {
            PrimitiveRenderer::drawOutlineRect(buttonX1 - 0.005f, buttonY1 - 0.005f,
                                             buttonX2 + 0.005f, buttonY2 + 0.005f,
                                             1.0f, 1.0f, 0.0f, 2.0f);
        }
    }

    glDisable(GL_SCISSOR_TEST);

    // Scroll bar for textures panel
    if (maxScroll > 0) {
        float scrollBarWidth = PrimitiveRenderer::pxToNDCx(8, width);
        float scrollBarX = panelX1 + panelWidth - scrollBarWidth - PrimitiveRenderer::pxToNDCx(5, width);
        float scrollBarHeight = panelHeight - PrimitiveRenderer::pxToNDCy(15, height);
        float scrollThumbHeight = scrollBarHeight * (panelHeight / contentHeight);
        float scrollProgress = state.leftPanelTexturesScroll / maxScroll;
        float scrollThumbY = panelY1 + panelHeight - PrimitiveRenderer::pxToNDCy(10, height) - scrollThumbHeight - (scrollProgress * (scrollBarHeight - scrollThumbHeight));

        PrimitiveRenderer::drawRect(scrollBarX, panelY1 + PrimitiveRenderer::pxToNDCy(10, height), scrollBarX + scrollBarWidth, panelY1 + panelHeight - PrimitiveRenderer::pxToNDCy(10, height), 0.3f, 0.3f, 0.3f);
        PrimitiveRenderer::drawRect(scrollBarX, scrollThumbY, scrollBarX + scrollBarWidth, scrollThumbY + scrollThumbHeight, 0.6f, 0.6f, 0.6f);
    }
}