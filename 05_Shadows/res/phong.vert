#version 410 core

layout(location = 0) in vec3 vertexPosition;  // vertex position in object/model space
layout(location = 1) in vec3 vertexNormal;    // vertex normal in object/model space

uniform mat4 MVP; // model-view-projection Matrix
uniform mat4 MV; // model-view Matrix
uniform mat4 MVit; // model-view inverse transpose Matrix

out vec3 posViewSpace;
out vec3 normalViewSpaceInterpolated;

void main()
{
  gl_Position = MVP * vec4(vertexPosition, 1);
  posViewSpace    = vec3(MV * vec4(vertexPosition, 1));
  normalViewSpaceInterpolated = normalize((MVit * vec4(vertexNormal, 0)).xyz);
}
