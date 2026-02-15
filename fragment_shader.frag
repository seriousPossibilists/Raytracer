#version 450 core

out vec4 FragColor;

uniform vec2 mousePos;
uniform int windowWidth;
uniform int windowHeight;
uniform vec3 cameraPos;

// OpenGL assumes column major matrices;
const float PI = 3.1415926535897932384;

bool intersectSphere(vec3 ro, vec3 rd, vec3 center, float radius, out float t)
{
    vec3 oc = ro - center;

    float a = dot(rd, rd);
    float b = 2.0 * dot(oc, rd);
    float c = dot(oc, oc) - radius * radius;

    float discriminant = b*b - 4.0*a*c;

    if (discriminant < 0.0)
        return false;

    t = (-b - sqrt(discriminant)) / (2.0 * a);
    return t > 0.0;
}

void main()
{
    vec2 pixelCoord = gl_FragCoord.xy;
    vec3 cameraPosition = cameraPos;
    float yaw =  PI * (2 * (mousePos[0] / float(windowWidth)) - 1);
    float pitch = (PI * 0.5) * (2.0 * (mousePos.y / float(windowHeight)) - 1.0);
    pitch = clamp(pitch, -PI * 0.5, PI * 0.5);

    mat4 R_x = mat4(
        1, 0, 0, 0,
        0, cos(pitch), -sin(pitch), 0,
        0, sin(pitch), cos(pitch), 0,
        0, 0, 0, 1
    );

    mat4 R_y = mat4(
        cos(yaw), 0, sin(yaw), 0, 
        0, 1, 0, 0,
        -sin(yaw), 0, cos(yaw), 0,
        0, 0, 0, 1
    );

    mat4 translation = mat4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        cameraPosition.x,
        cameraPosition.y,
        cameraPosition.z,
        1
    );

    mat4 modelMatrix = R_y * R_x * translation;
    vec2 uv = pixelCoord / vec2(windowWidth, windowHeight);
    uv = uv * 2.0 - 1.0;
    uv[0] *= (16.0 / 9.0);
    vec3 rayDir = vec3(uv, -1.0);
    rayDir = normalize(rayDir);
    vec3 worldRayDir = normalize((modelMatrix * vec4(rayDir, 0.0)).xyz);
    float t;
    if(intersectSphere(cameraPosition, worldRayDir, vec3(0.0, 1.0, -5.0), 1.0, t))
    {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
     }
     else
     {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
     }
}
