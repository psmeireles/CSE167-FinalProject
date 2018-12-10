#version 330 core
// This is a sample fragment shader.

in vec3 Normal;
in vec3 Color;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 FragColor;

uniform vec3 color;

void main()
{    
	FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	FragColor = vec4(Color, 1.0f);
}