//glsl version 4.5
#version 450

layout (binding = 0) uniform sampler2DArray samplerArray;

layout (location = 0) in vec3 inUV;

//output write
layout (location = 0) out vec4 outFragColor;

void main()	
{
	outFragColor = texture(samplerArray, inUV);
}