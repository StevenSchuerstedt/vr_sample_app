#version 420 core


layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord;

out vec2 Texcoord;

uniform mat4 mvp;

void main(){

Texcoord = texcoord; 
gl_Position = mvp * vec4(position.xyz, 1.0);

}