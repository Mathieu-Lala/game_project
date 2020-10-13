#version 330 core

in vec3 OutColor;

out vec4 FragColor;

void main()
{
    // this will draw a grid on the shape (and not on the full screen as wanted)
    if (int(gl_FragCoord.x) % 10 != 0 && int(gl_FragCoord.y) % 10 != 0)
        FragColor = vec4(OutColor, 1.0f);
    else
        FragColor = vec4(0, 0, 0, 1.0f);
}
