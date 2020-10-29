#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec3 OutColor;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 viewProj;

uniform bool shake;
uniform float time;

void main()
{
    OutColor = aColor;
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
    gl_Position = viewProj * model * vec4(aPos, 1.0f);
    if (shake) {
        float strength = 0.01;
        gl_Position.x += cos(time / 1000 * 10) * strength;
        gl_Position.y += cos(time / 1000 * 15) * strength;
    }
}
