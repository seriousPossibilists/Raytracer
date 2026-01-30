#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include "vec3.h"

const int width = 800;
const int height = 800;

std::vector<Vec3> raytracer_data(width * height);

Vec3 pixel_process(Vec3 coord)
{
    // (bx^2 + by^2)t^2 + 2(axbx + ayby)t + (ax^2+ay^2-r^2) = 0
    Vec3 sphereCenter; sphereCenter.x = 0.0; sphereCenter.y = 0.0; sphereCenter.z = 0.0;
    Vec3 lightDirection; lightDirection.x = 1.0; lightDirection.y = 1.0; lightDirection.z = 1.0;
    lightDirection = normalize(lightDirection);
    double radius = 0.5;

    Vec3 rayDirection; 
    rayDirection.x = (coord.x / static_cast<double>(width));
    rayDirection.y = (coord.y / static_cast<double>(height)); rayDirection.z = -1.0;
    rayDirection = 2.0 * rayDirection; rayDirection.x--; rayDirection.y--;
    rayDirection = normalize(rayDirection);

    Vec3 rayOrigin; rayOrigin.x = 0.0; rayOrigin.y = 0.0; rayOrigin.z = 2.0;

    double a = dot(rayDirection, rayDirection);
    double b = 2.0 * dot(rayOrigin - sphereCenter, rayDirection);
    double c = dot(rayOrigin - sphereCenter, rayOrigin - sphereCenter) - (radius * radius);
    Vec3 result; result.x = 0.0; result.y = 0.0; result.z = 0.0;

    // Solving for the hit point (assuming closest hitpoint as valid)
    // Sphere normal is hitpoint - center

    double Δ = (b*b) - (4.0*a*c);
    if (Δ < 0) {return result;}
    double t = (-b - std::sqrt(Δ)) / (2.0*a);

    if (t > 0)
    {
        Vec3 normal = (rayOrigin + (t*rayDirection)) - sphereCenter;
        normal = normalize(normal);
        if(dot(lightDirection, normal) > 0)
        {
            result.y = 255.0 * dot(lightDirection, normal);
        }
        else { result.y = 0; }
        result.x = 0;
        result.z = 0;
    }
    return result;
}

void raytrace()
{
    for(unsigned int i = 0; i < width; ++i)
    {
        for(unsigned int j = 0; j < height; ++j)
        {
            Vec3 coord;
            coord.x = (double)i;
            coord.y = (double)j;
            raytracer_data[(width * j) + i] = pixel_process(coord);
        }
    }
};




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

GLuint create_program(const char* vs, const char* fs) {
    GLuint v = compile_shader(GL_VERTEX_SHADER, vs);
    GLuint f = compile_shader(GL_FRAGMENT_SHADER, fs);
    GLuint program = glCreateProgram();
    glAttachShader(program, v);
    glAttachShader(program, f);
    glLinkProgram(program);
    glDeleteShader(v);
    glDeleteShader(f);
    return program;
}

const char* vertexShaderSrc = R"(#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;

out vec2 TexCoord;

void main() {
    TexCoord = aUV;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

const char* fragmentShaderSrc = R"(#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D screenTexture;

void main() {
    FragColor = texture(screenTexture, TexCoord);
}
)";

int main() {
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
    raytrace();
    for(unsigned int i = 0; i < raytracer_data.size(); i++)
    {
        pixels.push_back((int)raytracer_data[i].x);
        pixels.push_back((int)raytracer_data[i].y);
        pixels.push_back((int)raytracer_data[i].z);
    };

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLuint shader = create_program(vertexShaderSrc, fragmentShaderSrc);
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
