#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <ctime>

double mouseX, mouseY;
const int windowWidth = 1000; const double aspect = 16.0 / 9.0;
const int windowHeight = static_cast<int>((1 / aspect) * windowWidth);
double cameraX, cameraY, cameraZ;

static std::string read_shader_from_source(const char* pathToFile)
{
    std::ifstream file_stream(pathToFile);
    if (!file_stream.is_open())
    {
        std::cerr << "Could not read file from source\n";
        return "";
    }

    std::stringstream ss;
    ss << file_stream.rdbuf();
    return ss.str();
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (mouseX != xpos)
    {
        mouseX = xpos;
    }
    if (mouseY != ypos)
    {
        mouseY = ypos;
    }
}

int main()
{
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Window", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    // Load OpenGL via GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    glViewport(0, 0, windowWidth, windowHeight);

    std::string stringVertexSource = read_shader_from_source("vertex_shader.vert");
    const char* vertexShaderSource = stringVertexSource.c_str();
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed:\n" << infoLog << std::endl;
    }


    std::string stringFragmentSource = read_shader_from_source("fragment_shader.frag");
    const char* fragmentShaderSource = stringFragmentSource.c_str();
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed:\n" << infoLog << std::endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    clock_t startTime = clock();
    int frameCount = 0;
    double fps = 0.0;
    int mousePosUniformLoc = glGetUniformLocation(shaderProgram, "mousePos");
    int windowWidthUniformLoc = glGetUniformLocation(shaderProgram, "windowWidth");
    int windowHeightUniformLoc = glGetUniformLocation(shaderProgram, "windowHeight");
    int cameraPosUniformLoc = glGetUniformLocation(shaderProgram, "cameraPos");
    while (!glfwWindowShouldClose(window))
    {
        frameCount++;
        clock_t currentTime = clock();
        if (currentTime - startTime >= CLOCKS_PER_SEC)
        {
            fps = frameCount;
            frameCount = 0;
            startTime = currentTime;
            std::cout << fps << "\n";
        }
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glUniform2f(mousePosUniformLoc, mouseX, mouseY);
        glUniform1i(windowWidthUniformLoc, windowWidth);
        glUniform1i(windowHeightUniformLoc, windowHeight);
        glUniform3f(cameraPosUniformLoc, cameraX, cameraY, cameraZ);
            
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cameraZ -= 0.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraX -= 0.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraZ += 0.01f;   
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraX += 0.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            cameraY += 0.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            cameraY -= 0.01f;
        }
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
