#version 330 core
// This is a sample fragment shader.

in vec3 Normal;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 FragColor;

uniform vec3 color;

void main()
{    
	FragColor = vec4(Normal, 1.0);
}