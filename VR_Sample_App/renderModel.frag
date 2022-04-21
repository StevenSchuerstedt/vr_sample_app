#version 420 core

out vec4 frag_color;

in vec2 Texcoord;

uniform sampler2D tex;

void main(){

frag_color = texture(tex, Texcoord);

//frag_color = vec4(0.0, 1.0, 0.0, 1.0);
}