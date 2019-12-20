#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 inColor;

out vec4 myColor;

void main()
{
    gl_Position = vec4(aPos.x, -aPos.y, aPos.z, 1.0);
	myColor = vec4(inColor, 1.0f);
}