#version 410 core

layout(location = 0) in vec3 vertexPosition;  // vertex position in object/model space

uniform mat4 MVP; // model-view-projection Matrix

void main()
{
	gl_Position = PMV * vec4(vertexPosition, 1);
}
