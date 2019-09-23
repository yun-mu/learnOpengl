#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>

#include "shader.h"

using namespace std;

#define PI 3.141592653589793238463

void framebuffer_size_callback(GLFWwindow * window, int width, int height);
void processInput(GLFWwindow* window);
void initGlfwWindow();

// 屏幕宽，高
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

const unsigned int Y_SEGMENTS = 50;
const unsigned int X_SEGMENTS = 50;


int main() {
    initGlfwWindow();

    // 创建窗口(宽、高、窗口名称)
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Sphere", NULL, NULL);
    if (window == NULL) {
        // 如果窗口创建失败，输出Failed to Create OpenGL Context
        cout << "Failed to Create OpenGL Context" << endl;
        glfwTerminate();
        return -1;
    }

    // 将窗口的上下文设置为当前线程的主上下文
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 初始化GLAD，加载OpenGL函数指针地址的函数
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    Shader ourShader("VertexShader.hlsl", "FragShader.hlsl");

    vector<float> sphereVertices;
    // 生成球的顶点
    for (int y = 0; y <= Y_SEGMENTS; y++)
    {
        for (int x = 0; x <= X_SEGMENTS; x++)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = cos(xSegment * 2.0f * PI) * sin(ySegment * PI);
            float yPos = cos(ySegment * PI);
            float zPos = sin(xSegment * 2.0f * PI) * sin(ySegment * PI);
            sphereVertices.push_back(xPos);
            sphereVertices.push_back(yPos);
            sphereVertices.push_back(zPos);
        }
    }

    vector<unsigned int> sphereIndices;

    //根据球面上每一点的坐标，去构造一个一个三角形顶点数组
    for (int i = 0; i < Y_SEGMENTS; i++)
    {
        for (int j = 0; j < X_SEGMENTS; j++)
        {
            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);

            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j + 1);
        }
    }

    float* vertices = new float[sphereVertices.size()];
    memcpy(vertices, &sphereVertices[0], sphereVertices.size() * sizeof(float));
    unsigned int* indices = new unsigned int[sphereIndices.size()];
    memcpy(indices, &sphereIndices[0], sphereIndices.size() * sizeof(unsigned int));


    // 生成并绑定VAO和VBO
    GLuint VAO; // == VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO; // == VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // 将顶点数据绑定至当前默认的缓冲中
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), vertices, GL_STATIC_DRAW);

    GLuint EBO; // == EBO
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), indices, GL_STATIC_DRAW);


    // 设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 解绑VAO和VBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // 线框模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //开启面剔除(只需要展示一个面，否则会有重合)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        // 处理输入 
        processInput(window);

        // 清空颜色缓冲
        glClearColor(0.0f, 0.34f, 0.57f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 使用着色器程序
        ourShader.use();

        // 绘制球体
        glBindVertexArray(VAO);                                    // 绑定VAO
        glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);                                                      // 解除绑定

        // 交换缓冲并且检查是否有触发事件(比如键盘输入、鼠标移动等）
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 删除VAO和VBO
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // 清理所有的资源并正确退出程序
    glfwTerminate();

    // 释放空间
    delete[]vertices;
    delete[]indices;

    return 0;
};

void initGlfwWindow() {
    // 初始化GLFW
    glfwInit();                                                     // 初始化GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                  // OpenGL版本为3.3，主次版本号均设为3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 使用核心模式(无需向后兼容性)
    // 如果使用的是Mac OS X系统，需加上这行
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           
    glfwWindowHint(GLFW_RESIZABLE, false);						    // 不可改变窗口大小
};

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
};