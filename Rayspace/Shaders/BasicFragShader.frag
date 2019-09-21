#version 450 core

in vec4 VertColorExport;

void main(){
	gl_FragColor = vec4(VertColorExport);
}