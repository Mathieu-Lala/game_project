#version 330 core

in vec3 OutColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(OutColor, 1.0f);
}
