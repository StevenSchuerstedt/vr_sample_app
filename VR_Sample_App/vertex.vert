#version 420 core

attribute vec3 position;
attribute vec3 color;
attribute vec2 texcoord;
attribute vec3 normal;

out vec3 frag_pos;
out vec3 normal_pos;

uniform mat4 mvp;
uniform mat4 model;

void main(){

frag_pos = vec3(model * vec4(position, 1.0f));
normal_pos = mat3(transpose(inverse(model))) * normal;


gl_Position =  mvp * vec4(position, 1.0);

}