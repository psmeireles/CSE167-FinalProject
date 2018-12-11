#version 330 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform float time;

out vec3 FragNormal;
out vec3 FragPos;

//shader constants
const float amplitude = 0.3;
const float frequency = 10;
const float PI = 3.1415926535897932384626433832795;



void main()
{
    // Create waves
    
    //Get distance of current vertex from mesh center
    float dist = length(vertex);
    
    //Create a sin/cos function using the distance, multiply frequency and add the time
    float y = amplitude*sin(-PI*dist*frequency+time) +  amplitude*cos(-PI*dist*frequency+time);
    
    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
    gl_Position = projection * view * model * vec4(vertex, 1);
    
    //Update variables to pass to the fragment shader.
    FragPos = vec3(model * vec4(vertex.x, y, vertex.z, 1.0f));
    FragNormal = mat3(transpose(inverse(model))) * normal;
}
