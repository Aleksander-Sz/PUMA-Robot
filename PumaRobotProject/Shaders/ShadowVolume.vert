#version 330 core
layout(location = 0) in vec4 aPosH;
uniform mat4 viewProj;
void main()
{
    gl_Position = viewProj * aPosH;
}
