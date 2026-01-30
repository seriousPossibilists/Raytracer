#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <cmath>

const int width = 800;
const int height = 800;

struct Vec3 {
    double x {};
    double y {};
    double z {};
};

struct Ray {
    Vec3 origin {};
    Vec3 dir {};
};

Vec3 vec_add(Vec3 u, Vec3 v)
{
    Vec3 result {}; 
    result.x = u.x + v.x;
    result.y = u.y + v.y;
    result.z = u.z + v.z;
    return result;
};

Vec3 vec_subtract(Vec3 u, Vec3 v)
{
    Vec3 result {};
    result.x = u.x - v.x;
    result.y = u.y - v.y;
    result.z = u.z - v.z;
    return result;
};

Vec3 vec_normalize(Vec3 u)
{
    Vec3 result {};
    double length = std::sqrt((u.x * u.x) + (u.y * u.y) + (u.z * u.z));
    result.x = (u.x / length);
    result.y = (u.y / length);
    result.z = (u.z / length);
    return result;
};

double vec_dot(Vec3 u, Vec3 v)
{
    return (u.x * v.x) + (u.y * v.y) + (u.z * v.z);
}

Vec3 scalar_vec_prod(double t, Vec3 v)
{
    Vec3 result {};
    result.x = v.x * t;
    result.y = v.y * t;
    result.z = v.z * t;
    return result;
};

std::vector<Vec3> raytracer_data(width * height);

Vec3 raytracer_process(Vec3 coord)
{
    // (bx^2 + by^2)t^2 + 2(axbx + ayby)t + (ax^2+ay^2-r^2) = 0
    Vec3 sphereCenter; sphereCenter.x = 0.f; sphereCenter.y = 0.f; sphereCenter.z = 0.f;
    Vec3 lightDirection; lightDirection.x = 1; lightDirection.y = 1; lightDirection.z = 1;
    lightDirection = vec_normalize(lightDirection);
    double radius = 0.5f;
    Vec3 rayDirection;
    rayDirection.x = (coord.x / width);
    rayDirection.y = (coord.y / height);
    rayDirection.x = (2*rayDirection.x) - 1;
    rayDirection.y = (2*rayDirection.y) - 1;
    rayDirection.z = -1.f;
    rayDirection = vec_normalize(rayDirection);
    Vec3 rayOrigin; rayOrigin.x = 0.f; rayOrigin.y = 0.f; rayOrigin.z = 2.f;
    double a = vec_dot(rayDirection, rayDirection);
    double b = 2.f * vec_dot(rayOrigin, rayDirection);
    double c = vec_dot(rayOrigin, rayOrigin) - (radius * radius);
    Vec3 result; result.x = 0.f; result.y = 0.f; result.z = 0.f;
    // Solving for the hit point (assuming closest hitpoint as valid)
    // Sphere normal is hitpoint - center
    double Δ = (b*b) - (4*a*c);
    if (Δ < 0) {return result;}
    double t = (-b - std::sqrt(Δ)) / (2*a);
    if (t > 0)
    {
        Vec3 normal = vec_subtract(vec_add(rayOrigin, scalar_vec_prod(t, rayDirection)), sphereCenter);
        normal = vec_normalize(normal);
        if(vec_dot(lightDirection, normal) > 0)
        {
            result.x = 255 * (vec_dot(lightDirection, normal));
        }
        else { result.x = 0; }
        result.y = 0;
        result.z = 0;
    }
    return result;
}

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

    window = glfwCreateWindow(width, height, "Pixel Texture", nullptr, nullptr);
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

    for(unsigned int i = 0; i < width; ++i)
    {
        for(unsigned int j = 0; j < height; ++j)
        {
            Vec3 coord;
            coord.x = (double)i;
            coord.y = (double)j;
            raytracer_data[(width * j) + i] = raytracer_process(coord);
        }
    }

    for(unsigned int i = 0; i < raytracer_data.size(); i++)
    {
        pixels.push_back((int)raytracer_data[i].x);
        pixels.push_back((int)raytracer_data[i].y);
        pixels.push_back((int)raytracer_data[i].z);
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        width,
        height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        pixels.data()
    );

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
