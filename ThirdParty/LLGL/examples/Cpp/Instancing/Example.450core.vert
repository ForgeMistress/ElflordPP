// GLSL vertex shader
#version 450

layout(std140, binding = 0) uniform Settings
{
	mat4 vpMatrix;
	vec2 animationVector;
};

// Per-vertex attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

// Per-instance attributes
layout(location = 2) in vec3 color;
layout(location = 3) in float arrayLayer;
layout(location = 4) in mat4 wMatrix;

// Vertex output to the fragment shader
layout(location = 0) out vec3 vTexCoord;
layout(location = 1) out vec3 vColor;

out gl_PerVertex
{
    vec4 gl_Position;
};

// Vertex shader main function
void main()
{
	vec2 offset = animationVector * position.y;
	
	vec4 coord = vec4(
		position.x + offset.x,
		position.y,
		position.z + offset.y,
		1.0
	);
	
	gl_Position = vpMatrix * wMatrix * coord;
	
	vTexCoord = vec3(texCoord, arrayLayer);
	
	vColor = color;
}
