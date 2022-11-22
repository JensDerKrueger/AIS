#version 410 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 texCoords;

uniform mat4 MVP; // model-view-projection Matrix

out vec2 tc;

void main()
{
  gl_Position = MVP * vec4(vertexPosition, 1);
  tc = texCoords;
}
