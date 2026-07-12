in vec3 vertexPosition;
in vec3 vertexNormal;
in vec3 vertexTangent;
in vec3 vertexBinormal;
in vec2 vertexTexCoords;

uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 MVit;
uniform float texCoordScale;

out vec3 positionViewSpace;
out vec3 normalViewSpace;
out vec3 tangentViewSpace;
out vec3 binormalViewSpace;
out vec2 texCoords;

void main() {
  gl_Position = MVP * vec4(vertexPosition, 1.0);
  positionViewSpace = vec3(MV * vec4(vertexPosition, 1.0));
  normalViewSpace = normalize((MVit * vec4(vertexNormal, 0.0)).xyz);
  tangentViewSpace = normalize((MVit * vec4(vertexTangent, 0.0)).xyz);
  binormalViewSpace = normalize((MVit * vec4(vertexBinormal, 0.0)).xyz);
  texCoords = vertexTexCoords * texCoordScale;
}
