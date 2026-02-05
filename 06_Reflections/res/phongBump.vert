in vec3 vertexPosition;
in vec3 vertexNormal;
in vec3 vertexTangent;
in vec3 vertexBinormal;
in vec2 vertexTexCoords;

uniform mat4 MVP; // model-view-projection Matrix
uniform mat4 MV; // model-view Matrix
uniform mat4 M; // model matrix
uniform mat4 MVit; // model-view inverse transpose Matrix
uniform mat4 worldToShadow;

out vec3 posViewSpaceInterpolated;
out vec3 normalViewSpaceInterpolated;
out vec3 tangentViewSpaceInterpolated;
out vec3 binormViewSpaceInterpolated;
out vec2 texCoordsInterpolated;
out vec4 shadowPos;

void main() {
  gl_Position = MVP * vec4(vertexPosition, 1);
  posViewSpaceInterpolated    = vec3(MV * vec4(vertexPosition, 1));

  normalViewSpaceInterpolated = normalize((MVit * vec4(vertexNormal, 0)).xyz);
  tangentViewSpaceInterpolated = normalize((MVit * vec4(vertexTangent, 0)).xyz);;
  binormViewSpaceInterpolated = normalize((MVit * vec4(vertexBinormal, 0)).xyz);;
  texCoordsInterpolated = vertexTexCoords;
  shadowPos = worldToShadow * M * vec4(vertexPosition, 1);
}
