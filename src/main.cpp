#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <algorithm>

// Window settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Bar variables (pixel units)
float leftBarWidth = 150.0f;
float rightBarWidth = 150.0f;
float topBarHeight = 80.0f;

// Handle size (pixels)
const float HANDLE_PX = 10.0f;

bool resizingLeft = false;
bool resizingRight = false;
bool resizingTop = false;

double lastX, lastY;

// Function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

// Draw helpers
void drawRect(float x1, float y1, float x2, float y2, float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

void drawTriangle(float ax, float ay, float bx, float by, float cx, float cy, float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLES);
    glVertex2f(ax, ay);
    glVertex2f(bx, by);
    glVertex2f(cx, cy);
    glEnd();
}

int main()
{
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "BlenderLite", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    glClearColor(0.12f, 0.12f, 0.15f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        // NDC positions for bar edges
        float leftEdgeNDC = -1.0f + (2.0f * leftBarWidth / width);
        float rightEdgeNDC = 1.0f - (2.0f * rightBarWidth / width);
        float topEdgeNDC = 1.0f - (2.0f * topBarHeight / height);

        // Handle sizes in NDC
        float handleNDC_W = 2.0f * HANDLE_PX / width;
        float handleNDC_H = 2.0f * HANDLE_PX / height;
        float topLimitNDC = topEdgeNDC; // bottom of top bar

        float handleColor = 0.7f;

        // --- Draw side bars (stop at bottom of top bar) ---
        drawRect(-1.0f, -1.0f, leftEdgeNDC, topLimitNDC, 0.5f, 0.5f, 0.5f);   // left bar
        drawRect(rightEdgeNDC, -1.0f, 1.0f, topLimitNDC, 0.5f, 0.5f, 0.5f);   // right bar

        // --- Draw top bar ---
        drawRect(-1.0f, topEdgeNDC, 1.0f, 1.0f, 0.45f, 0.45f, 0.45f);

        // --- Draw handles ---
        drawRect(leftEdgeNDC - handleNDC_W, -1.0f, leftEdgeNDC, topLimitNDC, handleColor, handleColor, handleColor); // left
        drawRect(rightEdgeNDC, -1.0f, rightEdgeNDC + handleNDC_W, topLimitNDC, handleColor, handleColor, handleColor); // right
        drawRect(-1.0f, topEdgeNDC, 1.0f, topEdgeNDC + handleNDC_H, handleColor, handleColor, handleColor); // top handle

        // --- Draw smaller handle arrows ---
        float triPx = std::min(6.0f, (float)std::min(width, height) * 0.02f);
        float triHalfW = 2.0f * triPx / width;
        float triHalfH = 2.0f * triPx / height;

        // Left arrow (>)
        float leftCx = leftEdgeNDC - handleNDC_W * 0.5f;
        float leftCy = topLimitNDC * 0.5f - 0.25f;
        drawTriangle(leftCx - triHalfW, leftCy - triHalfH,
                     leftCx - triHalfW, leftCy + triHalfH,
                     leftCx + triHalfW, leftCy,
                     0.12f, 0.12f, 0.12f);

        // Right arrow (<)
        float rightCx = rightEdgeNDC + handleNDC_W * 0.5f;
        float rightCy = topLimitNDC * 0.5f - 0.25f;
        drawTriangle(rightCx + triHalfW, rightCy - triHalfH,
                     rightCx + triHalfW, rightCy + triHalfH,
                     rightCx - triHalfW, rightCy,
                     0.12f, 0.12f, 0.12f);

        // Top arrow (V)
        float topCx = 0.0f;
        float topCy = topEdgeNDC + handleNDC_H * 0.5f;
        drawTriangle(topCx - triHalfW, topCy + triHalfH,
                     topCx + triHalfW, topCy + triHalfH,
                     topCx, topCy - triHalfH,
                     0.12f, 0.12f, 0.12f);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    float leftEdge = leftBarWidth;
    float rightEdge = width - rightBarWidth;
    float topEdge = topBarHeight;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        if (xpos >= leftEdge - HANDLE_PX && xpos <= leftEdge)
            resizingLeft = true;
        else if (xpos >= rightEdge && xpos <= rightEdge + HANDLE_PX)
            resizingRight = true;
        else if (ypos >= (topEdge - HANDLE_PX) && ypos <= topEdge)
            resizingTop = true;

        lastX = xpos;
        lastY = ypos;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        resizingLeft = resizingRight = resizingTop = false;
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (!resizingLeft && !resizingRight && !resizingTop) return;

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    double dx = xpos - lastX;
    double dy = ypos - lastY;
    lastX = xpos;
    lastY = ypos;

    if (resizingLeft)
        leftBarWidth = std::max(50.0, std::min(leftBarWidth + dx,
            (double)width - rightBarWidth - 100.0));
    else if (resizingRight)
        rightBarWidth = std::max(50.0, std::min(rightBarWidth - dx,
            (double)width - leftBarWidth - 100.0));
    else if (resizingTop)
        topBarHeight = std::max(40.0, std::min(topBarHeight + dy,
            (double)height - 100.0));
}
