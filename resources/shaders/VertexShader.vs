#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 fColor;  

uniform mat4 world_t;
uniform mat4 view_t;
uniform mat4 projection_t;

void main(){
    gl_Position = projection_t * view_t * world_t * vec4(aPos, 1.0f); 
    fColor = aColor;
}