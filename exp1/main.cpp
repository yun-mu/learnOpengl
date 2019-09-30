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

// ��Ļ����
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

const unsigned int Y_SEGMENTS = 50;
const unsigned int X_SEGMENTS = 50;


int main() {
    initGlfwWindow();

    // ��������(���ߡ���������)
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Sphere", NULL, NULL);
    if (window == NULL) {
        // ������ڴ���ʧ�ܣ����Failed to Create OpenGL Context
        cout << "Failed to Create OpenGL Context" << endl;
        glfwTerminate();
        return -1;
    }

    // �����ڵ�����������Ϊ��ǰ�̵߳���������
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ��ʼ��GLAD������OpenGL����ָ���ַ�ĺ���
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    Shader ourShader("VertexShader.hlsl", "FragShader.hlsl");

    vector<float> sphereVertices;
    // ������Ķ���
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

    //����������ÿһ������꣬ȥ����һ��һ�������ζ�������
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


    // ���ɲ���VAO��VBO
    GLuint VAO; // == VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO; // == VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // ���������ݰ�����ǰĬ�ϵĻ�����
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), vertices, GL_STATIC_DRAW);

    GLuint EBO; // == EBO
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), indices, GL_STATIC_DRAW);


    // ���ö�������ָ��
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // ���VAO��VBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // �߿�ģʽ
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //�������޳�(ֻ��Ҫչʾһ���棬��������غ�)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // ��Ⱦѭ��
    while (!glfwWindowShouldClose(window)) {
        // �������� 
        processInput(window);

        // �����ɫ����
        glClearColor(0.0f, 0.34f, 0.57f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // ʹ����ɫ������
        ourShader.use();

        // ��������
        glBindVertexArray(VAO);                                    // ��VAO
        glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);                                                      // �����

        // �������岢�Ҽ���Ƿ��д����¼�(����������롢����ƶ��ȣ�
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ɾ��VAO��VBO
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // �������е���Դ����ȷ�˳�����
    glfwTerminate();

    // �ͷſռ�
    delete[]vertices;
    delete[]indices;

    return 0;
};

void initGlfwWindow() {
    // ��ʼ��GLFW
    glfwInit();                                                     // ��ʼ��GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                  // OpenGL�汾Ϊ3.3�����ΰ汾�ž���Ϊ3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // ʹ�ú���ģʽ(������������)
    // ���ʹ�õ���Mac OS Xϵͳ�����������
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           
    glfwWindowHint(GLFW_RESIZABLE, false);						    // ���ɸı䴰�ڴ�С
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