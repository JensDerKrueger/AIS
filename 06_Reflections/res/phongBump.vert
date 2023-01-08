#version 410 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec3 vertexTangent;
layout(location = 3) in vec3 vertexBinormal;
layout(location = 4) in vec2 vertexTexCoords;

uniform mat4 MVP; // model-view-projection Matrix
uniform mat4 MV; // model-view Matrix
uniform mat4 M; // model matrix
uniform mat4 MVit; // model-view inverse transpose Matrix
uniform mat4 worldToShadow;

out vec3 posViewSpaceInterpolated;
out vec3 normalViewSpaceInterpolated;
out vec3 tangentViewSpaceInterpolated;
out vec3 binormtViewSpaceInterpolated;
out vec2 texCoordsInterpolated;
out vec4 shadowPos;

void main() {
  gl_Position = MVP * vec4(vertexPosition, 1);
  posViewSpaceInterpolated    = vec3(MV * vec4(vertexPosition, 1));

  normalViewSpaceInterpolated = normalize((MVit * vec4(vertexNormal, 0)).xyz);
  tangentViewSpaceInterpolated = normalize((MVit * vec4(vertexTangent, 0)).xyz);;
  binormtViewSpaceInterpolated = normalize((MVit * vec4(vertexBinormal, 0)).xyz);;
  texCoordsInterpolated = vertexTexCoords;
  shadowPos = worldToShadow * M * vec4(vertexPosition, 1);
}
