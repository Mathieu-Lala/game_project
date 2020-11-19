#version 330 core

in vec3 OutColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D ourTexture;

void main()
{
    vec4 color = texture(ourTexture, TexCoord) * vec4(OutColor, 1.0);
    if (color.a < 0.1)
        discard;
    FragColor = color;
}
