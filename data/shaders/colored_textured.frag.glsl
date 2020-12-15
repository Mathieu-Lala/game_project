#version 450 core

in vec4 OutColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D ourTexture;

void main()
{
    vec4 color = texture(ourTexture, TexCoord) * OutColor;
    if (color.a < 0.05)
        discard;
    FragColor = color;
}
