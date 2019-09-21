#version 450 core

in vec4 VertCoords;
in vec4 VertColor;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec4 VertColorExport;

void main(void){
	VertColorExport = VertColor;
	gl_Position = Projection * View * Model * VertCoords;
}