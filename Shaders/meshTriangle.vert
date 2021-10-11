#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 uvTextureCoord;

layout (location = 0) out vec3 textureCoord;

layout (push_constant) uniform constants
{
	vec2 screenDimensions;
	vec2 tileOffset;
	float sizeOfTile;
	uint characterLayer;
} PushConstants;

void main()
{
	//we send the sprite offset in spriteCoordX and spriteCoordY
	gl_Position = vec4(vPosition.xy + ((PushConstants.tileOffset * PushConstants.sizeOfTile) / PushConstants.screenDimensions), vPosition.z, 1.0f);
	textureCoord = vec3(uvTextureCoord, PushConstants.characterLayer);
}