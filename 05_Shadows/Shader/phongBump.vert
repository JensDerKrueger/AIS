in vec3 vertexPosition;
in vec3 vertexNormal;
in vec3 vertexTangent;
in vec3 vertexBinormal;
in vec2 vertexTexCoords;

uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 MVit;

out vec3 posViewSpaceInterpolated;
out vec3 normalViewSpaceInterpolated;
out vec3 tangentViewSpaceInterpolated;
out vec3 binormtViewSpaceInterpolated;
out vec2 texCoordsInterpolated;

void main() {
  gl_Position = MVP * vec4(vertexPosition, 1.0);
  posViewSpaceInterpolated    = vec3(MV * vec4(vertexPosition, 1.0));

  normalViewSpaceInterpolated = normalize((MVit * vec4(vertexNormal, 0.0)).xyz);
  tangentViewSpaceInterpolated = normalize((MVit * vec4(vertexTangent, 0.0)).xyz);;
  binormtViewSpaceInterpolated = normalize((MVit * vec4(vertexBinormal, 0.0)).xyz);;
  texCoordsInterpolated = vertexTexCoords;
}
