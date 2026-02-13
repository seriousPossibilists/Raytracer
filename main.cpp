#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>

#include "vec3.h"
#include "sphere.h"
#include "ray.h"
#include "camera.h"

const char* shaderArr[] = {"", ""};

// Shader compilation
GLuint compile_shader(GLenum type, const char* src)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) 
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compile error: \n" << infoLog << "\n";
    }
    return shader;
}

std::string read_shader_from_source(const char* pathToFile)
{
    std::ifstream file_stream(pathToFile);
    if(!file_stream.is_open())
    {
        std::cerr << "Could not read file from source\n";
        return "";
    }

    std::stringstream ss;
    ss << file_stream.rdbuf();
    return ss.str();
}


GLuint create_program()
{
    std::string vertexSrc = read_shader_from_source("src/vertex_shader.vert");
    std::string fragmentSrc = read_shader_from_source("src/fragment_shader.frag");

    GLuint v = compile_shader(GL_VERTEX_SHADER, vertexSrc.c_str());
    GLuint f = compile_shader(GL_FRAGMENT_SHADER, fragmentSrc.c_str());

    GLuint program = glCreateProgram();
    glAttachShader(program, v);
    glAttachShader(program, f);
    glLinkProgram(program);

    glDeleteShader(v);
    glDeleteShader(f);

    return program;
}

int main() {

    Sphere floor; floor.center = { 0.0, -1001.0, -3.0 }; floor.color = { 0.8, 0.8, 0.8 }; floor.radius = 1000.0; 
    spheres.push_back(floor);

    Sphere a; a.center = { -1.6, 0.0, 5.5 }; a.color = { 1.0, 0.2, 0.2 }; a.radius = 0.5; spheres.push_back(a);
    Sphere b; b.center = {  0.0, 0.0, 5.0 }; b.color = { 0.2, 1.0, 0.2 }; b.radius = 0.5; spheres.push_back(b);
    Sphere c; c.center = {  0.8, 0.7, 5.8 }; c.color = { 0.2, 0.2, 1.0 }; c.radius = 0.5; spheres.push_back(c);

    Sphere d; d.center = { -2.5, 0.0, 5.7 }; d.color = { 1.0, 1.0, 0.2 }; d.radius = 0.3; spheres.push_back(d);
    Sphere e; e.center = {  0.5, 0.8, 5.1 }; e.color = { 0.2, 1.0, 1.0 }; e.radius = 0.3; spheres.push_back(e);

    Sphere light; light.center = { -7.5, 3.5, -5.0 }; light.color = { 1.0, 1.0, 1.0 }; light.radius = 5; 
    light.emissionColor = { 1.0, 1.0, 1.0 }; light.emissionStrength = 1.0; spheres.push_back(light);

    GLFWwindow* window;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(width, height, "Window", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD\n";
        return -1;
    }
    glViewport(0, 0, width, height);
    float quad[] = {
        // pos        // uv
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,

        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    std::vector<unsigned char> pixels;
    process();
    for(unsigned int i = 0; i < raytracer_data.size(); i++)
    {
        pixels.push_back(static_cast<int>(255.0 * raytracer_data[i].x));
        pixels.push_back(static_cast<int>(255.0 * raytracer_data[i].y));
        pixels.push_back(static_cast<int>(255.0 * raytracer_data[i].z));
    };

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLuint shader = create_program();
    glUseProgram(shader);
    glUniform1i(glGetUniformLocation(shader, "screenTexture"), 0);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        // update pixels here if needed
        // glTexSubImage2D(...)
        glClear(GL_COLOR_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}
