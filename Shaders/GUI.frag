//glsl version 4.5
#version 450
precision highp float;

layout (location = 0) in float alpha;

//output write
layout (location = 0) out vec4 outFragColor;
float u_time;

void main()	
{
	outFragColor = vec4(1.0f, 1.0f, 1.0f, alpha);
}