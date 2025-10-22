#include "../include/ui/Panels.hpp"
#include "../include/rendering/PrimitiveRenderer.hpp"
#include <iostream>
#include <algorithm>
#include <cstdio>

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

    PrimitiveRenderer::drawRect(panelX1 + PrimitiveRenderer::pxToNDCx(15, width), inputY - inputBoxHeight,
             panelX1 + PrimitiveRenderer::pxToNDCx(15, width) + inputBoxWidth, inputY, 0.9f, 0.9f, 0.9f);
    PrimitiveRenderer::drawOutlineRect(panelX1 + PrimitiveRenderer::pxToNDCx(15, width), inputY - inputBoxHeight,
                   panelX1 + PrimitiveRenderer::pxToNDCx(15, width) + inputBoxWidth, inputY, 0.0f, 0.0f, 0.0f, 1.0f);

    glColor3f(0.2f, 0.2f, 0.2f);
    char xValue[20];
    snprintf(xValue, sizeof(xValue), "X: %7.3f", values[0]);
    PrimitiveRenderer::drawText(xValue, panelX1 + PrimitiveRenderer::pxToNDCx(20, width), inputY - PrimitiveRenderer::pxToNDCy(15, height), GLUT_BITMAP_HELVETICA_12);

    inputY -= inputBoxHeight + PrimitiveRenderer::pxToNDCy(2, height);
    PrimitiveRenderer::drawRect(panelX1 + PrimitiveRenderer::pxToNDCx(15, width), inputY - inputBoxHeight,
             panelX1 + PrimitiveRenderer::pxToNDCx(15, width) + inputBoxWidth, inputY, 0.9f, 0.9f, 0.9f);
    PrimitiveRenderer::drawOutlineRect(panelX1 + PrimitiveRenderer::pxToNDCx(15, width), inputY - inputBoxHeight,
                   panelX1 + PrimitiveRenderer::pxToNDCx(15, width) + inputBoxWidth, inputY, 0.0f, 0.0f, 0.0f, 1.0f);

    char yValue[20];
    snprintf(yValue, sizeof(yValue), "Y: %7.3f", values[1]);
    PrimitiveRenderer::drawText(yValue, panelX1 + PrimitiveRenderer::pxToNDCx(20, width), inputY - PrimitiveRenderer::pxToNDCy(15, height), GLUT_BITMAP_HELVETICA_12);

    inputY -= inputBoxHeight + PrimitiveRenderer::pxToNDCy(2, height);
    PrimitiveRenderer::drawRect(panelX1 + PrimitiveRenderer::pxToNDCx(15, width), inputY - inputBoxHeight,
             panelX1 + PrimitiveRenderer::pxToNDCx(15, width) + inputBoxWidth, inputY, 0.9f, 0.9f, 0.9f);
    PrimitiveRenderer::drawOutlineRect(panelX1 + PrimitiveRenderer::pxToNDCx(15, width), inputY - inputBoxHeight,
                   panelX1 + PrimitiveRenderer::pxToNDCx(15, width) + inputBoxWidth, inputY, 0.0f, 0.0f, 0.0f, 1.0f);

    char zValue[20];
    snprintf(zValue, sizeof(zValue), "Z: %7.3f", values[2]);
    PrimitiveRenderer::drawText(zValue, panelX1 + PrimitiveRenderer::pxToNDCx(20, width), inputY - PrimitiveRenderer::pxToNDCy(15, height), GLUT_BITMAP_HELVETICA_12);

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

    // Square button
    float squareX1 = panelX1 + PrimitiveRenderer::pxToNDCx(10, width);
    float squareX2 = squareX1 + buttonWidth;
    bool hoverSquare = PrimitiveRenderer::isInsideNDC(state.mouseX, state.mouseY, width, height, squareX1, row1Y - buttonHeight, squareX2, row1Y);
    PrimitiveRenderer::drawRect(squareX1, row1Y - buttonHeight, squareX2, row1Y, 0.3f + (hoverSquare ? 0.1f : 0.0f), 0.3f + (hoverSquare ? 0.1f : 0.0f), 0.3f + (hoverSquare ? 0.1f : 0.0f));
    PrimitiveRenderer::drawOutlineRect(squareX1, row1Y - buttonHeight, squareX2, row1Y, 0.0f, 0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText("Square", squareX1 + PrimitiveRenderer::pxToNDCx(20, width), row1Y - PrimitiveRenderer::pxToNDCy(35, height), GLUT_BITMAP_HELVETICA_12);
    PrimitiveRenderer::drawText("[generate", squareX1 + PrimitiveRenderer::pxToNDCx(15, width), row1Y - PrimitiveRenderer::pxToNDCy(55, height), GLUT_BITMAP_HELVETICA_12);
    PrimitiveRenderer::drawText("square]", squareX1 + PrimitiveRenderer::pxToNDCx(20, width), row1Y - PrimitiveRenderer::pxToNDCy(70, height), GLUT_BITMAP_HELVETICA_12);

    // Sphere button
    float sphereX1 = squareX2 + buttonGap;
    float sphereX2 = sphereX1 + buttonWidth;
    bool hoverSphere = PrimitiveRenderer::isInsideNDC(state.mouseX, state.mouseY, width, height, sphereX1, row1Y - buttonHeight, sphereX2, row1Y);
    PrimitiveRenderer::drawRect(sphereX1, row1Y - buttonHeight, sphereX2, row1Y, 0.3f + (hoverSphere ? 0.1f : 0.0f), 0.3f + (hoverSphere ? 0.1f : 0.0f), 0.3f + (hoverSphere ? 0.1f : 0.0f));
    PrimitiveRenderer::drawOutlineRect(sphereX1, row1Y - buttonHeight, sphereX2, row1Y, 0.0f, 0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText("Sphere", sphereX1 + PrimitiveRenderer::pxToNDCx(20, width), row1Y - PrimitiveRenderer::pxToNDCy(35, height), GLUT_BITMAP_HELVETICA_12);
    PrimitiveRenderer::drawText("[generate", sphereX1 + PrimitiveRenderer::pxToNDCx(15, width), row1Y - PrimitiveRenderer::pxToNDCy(55, height), GLUT_BITMAP_HELVETICA_12);
    PrimitiveRenderer::drawText("sphere]", sphereX1 + PrimitiveRenderer::pxToNDCx(20, width), row1Y - PrimitiveRenderer::pxToNDCy(70, height), GLUT_BITMAP_HELVETICA_12);

    float row2Y = row1Y - buttonHeight - rowGap;

    // Cone button
    float coneX1 = panelX1 + PrimitiveRenderer::pxToNDCx(10, width);
    float coneX2 = coneX1 + buttonWidth;
    bool hoverCone = PrimitiveRenderer::isInsideNDC(state.mouseX, state.mouseY, width, height, coneX1, row2Y - buttonHeight, coneX2, row2Y);
    PrimitiveRenderer::drawRect(coneX1, row2Y - buttonHeight, coneX2, row2Y, 0.3f + (hoverCone ? 0.1f : 0.0f), 0.3f + (hoverCone ? 0.1f : 0.0f), 0.3f + (hoverCone ? 0.1f : 0.0f));
    PrimitiveRenderer::drawOutlineRect(coneX1, row2Y - buttonHeight, coneX2, row2Y, 0.0f, 0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText("Cone", coneX1 + PrimitiveRenderer::pxToNDCx(25, width), row2Y - PrimitiveRenderer::pxToNDCy(35, height), GLUT_BITMAP_HELVETICA_12);
    PrimitiveRenderer::drawText("[generate", coneX1 + PrimitiveRenderer::pxToNDCx(15, width), row2Y - PrimitiveRenderer::pxToNDCy(55, height), GLUT_BITMAP_HELVETICA_12);
    PrimitiveRenderer::drawText("cone]", coneX1 + PrimitiveRenderer::pxToNDCx(25, width), row2Y - PrimitiveRenderer::pxToNDCy(70, height), GLUT_BITMAP_HELVETICA_12);

    // Cylinder button
    float cylinderX1 = coneX2 + buttonGap;
    float cylinderX2 = cylinderX1 + buttonWidth;
    bool hoverCylinder = PrimitiveRenderer::isInsideNDC(state.mouseX, state.mouseY, width, height, cylinderX1, row2Y - buttonHeight, cylinderX2, row2Y);
    PrimitiveRenderer::drawRect(cylinderX1, row2Y - buttonHeight, cylinderX2, row2Y, 0.3f + (hoverCylinder ? 0.1f : 0.0f), 0.3f + (hoverCylinder ? 0.1f : 0.0f), 0.3f + (hoverCylinder ? 0.1f : 0.0f));
    PrimitiveRenderer::drawOutlineRect(cylinderX1, row2Y - buttonHeight, cylinderX2, row2Y, 0.0f, 0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText("Cylinder", cylinderX1 + PrimitiveRenderer::pxToNDCx(15, width), row2Y - PrimitiveRenderer::pxToNDCy(35, height), GLUT_BITMAP_HELVETICA_12);
    PrimitiveRenderer::drawText("[generate", cylinderX1 + PrimitiveRenderer::pxToNDCx(15, width), row2Y - PrimitiveRenderer::pxToNDCy(55, height), GLUT_BITMAP_HELVETICA_12);
    PrimitiveRenderer::drawText("cylinder]", cylinderX1 + PrimitiveRenderer::pxToNDCx(15, width), row2Y - PrimitiveRenderer::pxToNDCy(70, height), GLUT_BITMAP_HELVETICA_12);

    float row3Y = row2Y - buttonHeight - rowGap;

    // Torus button
    float torusX1 = panelX1 + PrimitiveRenderer::pxToNDCx(10, width);
    float torusX2 = torusX1 + buttonWidth;
    bool hoverTorus = PrimitiveRenderer::isInsideNDC(state.mouseX, state.mouseY, width, height, torusX1, row3Y - buttonHeight, torusX2, row3Y);
    PrimitiveRenderer::drawRect(torusX1, row3Y - buttonHeight, torusX2, row3Y, 0.3f + (hoverTorus ? 0.1f : 0.0f), 0.3f + (hoverTorus ? 0.1f : 0.0f), 0.3f + (hoverTorus ? 0.1f : 0.0f));
    PrimitiveRenderer::drawOutlineRect(torusX1, row3Y - buttonHeight, torusX2, row3Y, 0.0f, 0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText("Torus", torusX1 + PrimitiveRenderer::pxToNDCx(25, width), row3Y - PrimitiveRenderer::pxToNDCy(35, height), GLUT_BITMAP_HELVETICA_12);
    PrimitiveRenderer::drawText("[generate", torusX1 + PrimitiveRenderer::pxToNDCx(15, width), row3Y - PrimitiveRenderer::pxToNDCy(55, height), GLUT_BITMAP_HELVETICA_12);
    PrimitiveRenderer::drawText("torus]", torusX1 + PrimitiveRenderer::pxToNDCx(25, width), row3Y - PrimitiveRenderer::pxToNDCy(70, height), GLUT_BITMAP_HELVETICA_12);

    // Pyramid button
    float pyramidX1 = torusX2 + buttonGap;
    float pyramidX2 = pyramidX1 + buttonWidth;
    bool hoverPyramid = PrimitiveRenderer::isInsideNDC(state.mouseX, state.mouseY, width, height, pyramidX1, row3Y - buttonHeight, pyramidX2, row3Y);
    PrimitiveRenderer::drawRect(pyramidX1, row3Y - buttonHeight, pyramidX2, row3Y, 0.3f + (hoverPyramid ? 0.1f : 0.0f), 0.3f + (hoverPyramid ? 0.1f : 0.0f), 0.3f + (hoverPyramid ? 0.1f : 0.0f));
    PrimitiveRenderer::drawOutlineRect(pyramidX1, row3Y - buttonHeight, pyramidX2, row3Y, 0.0f, 0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText("Pyramid", pyramidX1 + PrimitiveRenderer::pxToNDCx(20, width), row3Y - PrimitiveRenderer::pxToNDCy(35, height), GLUT_BITMAP_HELVETICA_12);
    PrimitiveRenderer::drawText("[generate", pyramidX1 + PrimitiveRenderer::pxToNDCx(15, width), row3Y - PrimitiveRenderer::pxToNDCy(55, height), GLUT_BITMAP_HELVETICA_12);
    PrimitiveRenderer::drawText("pyramid]", pyramidX1 + PrimitiveRenderer::pxToNDCx(17, width), row3Y - PrimitiveRenderer::pxToNDCy(70, height), GLUT_BITMAP_HELVETICA_12);

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

    float buttonWidth = (panelWidth - PrimitiveRenderer::pxToNDCx(40, width)) / 2.0f;
    float buttonHeight = PrimitiveRenderer::pxToNDCy(75, height);
    float buttonGap = PrimitiveRenderer::pxToNDCx(8, width);

    float rowGap = PrimitiveRenderer::pxToNDCy(10, height);
    float contentHeight = (buttonHeight * 3) + (rowGap * 2);
    float maxScroll = std::max(0.0f, contentHeight - panelHeight);

    float scrollOffset = std::max(0.0f, std::min(maxScroll, -state.leftPanelTexturesScroll));

    float row1Y = panelY1 + panelHeight - PrimitiveRenderer::pxToNDCy(50, height) + scrollOffset;

    // Texture 1 button
    float tex1X1 = panelX1 + PrimitiveRenderer::pxToNDCx(10, width);
    float tex1X2 = tex1X1 + buttonWidth;
    bool hoverTex1 = PrimitiveRenderer::isInsideNDC(state.mouseX, state.mouseY, width, height, tex1X1, row1Y - buttonHeight, tex1X2, row1Y);
    PrimitiveRenderer::drawRect(tex1X1, row1Y - buttonHeight, tex1X2, row1Y, 0.3f + (hoverTex1 ? 0.1f : 0.0f), 0.3f + (hoverTex1 ? 0.1f : 0.0f), 0.3f + (hoverTex1 ? 0.1f : 0.0f));
    PrimitiveRenderer::drawOutlineRect(tex1X1, row1Y - buttonHeight, tex1X2, row1Y, 0.0f, 0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText("[texture1.png]", tex1X1 + PrimitiveRenderer::pxToNDCx(8, width), row1Y - PrimitiveRenderer::pxToNDCy(45, height), GLUT_BITMAP_HELVETICA_12);

    // Texture 2 button
    float tex2X1 = tex1X2 + buttonGap;
    float tex2X2 = tex2X1 + buttonWidth;
    bool hoverTex2 = PrimitiveRenderer::isInsideNDC(state.mouseX, state.mouseY, width, height, tex2X1, row1Y - buttonHeight, tex2X2, row1Y);
    PrimitiveRenderer::drawRect(tex2X1, row1Y - buttonHeight, tex2X2, row1Y, 0.3f + (hoverTex2 ? 0.1f : 0.0f), 0.3f + (hoverTex2 ? 0.1f : 0.0f), 0.3f + (hoverTex2 ? 0.1f : 0.0f));
    PrimitiveRenderer::drawOutlineRect(tex2X1, row1Y - buttonHeight, tex2X2, row1Y, 0.0f, 0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText("[texture2.png]", tex2X1 + PrimitiveRenderer::pxToNDCx(8, width), row1Y - PrimitiveRenderer::pxToNDCy(45, height), GLUT_BITMAP_HELVETICA_12);

    float row2Y = row1Y - buttonHeight - rowGap;

    // Texture 3 button
    float tex3X1 = panelX1 + PrimitiveRenderer::pxToNDCx(10, width);
    float tex3X2 = tex3X1 + buttonWidth;
    bool hoverTex3 = PrimitiveRenderer::isInsideNDC(state.mouseX, state.mouseY, width, height, tex3X1, row2Y - buttonHeight, tex3X2, row2Y);
    PrimitiveRenderer::drawRect(tex3X1, row2Y - buttonHeight, tex3X2, row2Y, 0.3f + (hoverTex3 ? 0.1f : 0.0f), 0.3f + (hoverTex3 ? 0.1f : 0.0f), 0.3f + (hoverTex3 ? 0.1f : 0.0f));
    PrimitiveRenderer::drawOutlineRect(tex3X1, row2Y - buttonHeight, tex3X2, row2Y, 0.0f, 0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText("[texture3.png]", tex3X1 + PrimitiveRenderer::pxToNDCx(8, width), row2Y - PrimitiveRenderer::pxToNDCy(45, height), GLUT_BITMAP_HELVETICA_12);

    // Texture 4 button
    float tex4X1 = tex3X2 + buttonGap;
    float tex4X2 = tex4X1 + buttonWidth;
    bool hoverTex4 = PrimitiveRenderer::isInsideNDC(state.mouseX, state.mouseY, width, height, tex4X1, row2Y - buttonHeight, tex4X2, row2Y);
    PrimitiveRenderer::drawRect(tex4X1, row2Y - buttonHeight, tex4X2, row2Y, 0.3f + (hoverTex4 ? 0.1f : 0.0f), 0.3f + (hoverTex4 ? 0.1f : 0.0f), 0.3f + (hoverTex4 ? 0.1f : 0.0f));
    PrimitiveRenderer::drawOutlineRect(tex4X1, row2Y - buttonHeight, tex4X2, row2Y, 0.0f, 0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText("[texture4.png]", tex4X1 + PrimitiveRenderer::pxToNDCx(8, width), row2Y - PrimitiveRenderer::pxToNDCy(45, height), GLUT_BITMAP_HELVETICA_12);

    float row3Y = row2Y - buttonHeight - rowGap;

    // Texture 5 button
    float tex5X1 = panelX1 + PrimitiveRenderer::pxToNDCx(10, width);
    float tex5X2 = tex5X1 + buttonWidth;
    bool hoverTex5 = PrimitiveRenderer::isInsideNDC(state.mouseX, state.mouseY, width, height, tex5X1, row3Y - buttonHeight, tex5X2, row3Y);
    PrimitiveRenderer::drawRect(tex5X1, row3Y - buttonHeight, tex5X2, row3Y, 0.3f + (hoverTex5 ? 0.1f : 0.0f), 0.3f + (hoverTex5 ? 0.1f : 0.0f), 0.3f + (hoverTex5 ? 0.1f : 0.0f));
    PrimitiveRenderer::drawOutlineRect(tex5X1, row3Y - buttonHeight, tex5X2, row3Y, 0.0f, 0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText("[texture5.png]", tex5X1 + PrimitiveRenderer::pxToNDCx(8, width), row3Y - PrimitiveRenderer::pxToNDCy(45, height), GLUT_BITMAP_HELVETICA_12);

    // Texture 6 button
    float tex6X1 = tex5X2 + buttonGap;
    float tex6X2 = tex6X1 + buttonWidth;
    bool hoverTex6 = PrimitiveRenderer::isInsideNDC(state.mouseX, state.mouseY, width, height, tex6X1, row3Y - buttonHeight, tex6X2, row3Y);
    PrimitiveRenderer::drawRect(tex6X1, row3Y - buttonHeight, tex6X2, row3Y, 0.3f + (hoverTex6 ? 0.1f : 0.0f), 0.3f + (hoverTex6 ? 0.1f : 0.0f), 0.3f + (hoverTex6 ? 0.1f : 0.0f));
    PrimitiveRenderer::drawOutlineRect(tex6X1, row3Y - buttonHeight, tex6X2, row3Y, 0.0f, 0.0f, 0.0f, 1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    PrimitiveRenderer::drawText("[texture6.png]", tex6X1 + PrimitiveRenderer::pxToNDCx(8, width), row3Y - PrimitiveRenderer::pxToNDCy(45, height), GLUT_BITMAP_HELVETICA_12);

    glDisable(GL_SCISSOR_TEST);

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