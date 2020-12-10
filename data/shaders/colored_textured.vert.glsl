#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec4 OutColor;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 viewProj;

uniform bool shake;
uniform float time;

uniform bool mirrored;

void main()
{
    OutColor = aColor;
    if (mirrored)
        TexCoord = vec2(-aTexCoord.x , aTexCoord.y);
    else
        TexCoord = vec2(aTexCoord.x , aTexCoord.y);

    gl_Position = viewProj * model * vec4(aPos, 1.0f);
    if (shake) {
        float strength = 0.01;
        gl_Position.x += cos(time / 1000 * 10) * strength;
        gl_Position.y += cos(time / 1000 * 15) * strength;
    }
}
