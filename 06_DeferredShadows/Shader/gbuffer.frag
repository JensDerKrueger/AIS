in vec3 positionViewSpace;
in vec3 normalViewSpace;
in vec3 tangentViewSpace;
in vec3 binormalViewSpace;
in vec2 texCoords;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;

uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float shininess;
uniform int useDiffuseTexture;
uniform int useSpecularTexture;
uniform int useNormalTexture;

layout(location = 0) out vec4 outDiffuse;
layout(location = 1) out vec4 outSpecular;
layout(location = 2) out vec4 outNormal;
layout(location = 3) out vec4 outPosition;

void main() {
  vec3 diffuse = materialDiffuse;
  vec3 specular = materialSpecular;

  if(useDiffuseTexture != 0) {
    diffuse *= texture(diffuseTexture, texCoords).rgb;
  }

  if(useSpecularTexture != 0) {
    specular *= texture(specularTexture, texCoords).rgb;
  }

  vec3 N = normalize(normalViewSpace);

  if(useNormalTexture != 0) {
    vec3 T = normalize(tangentViewSpace);
    vec3 B = normalize(binormalViewSpace);
    vec3 normalMap = texture(normalTexture, texCoords).xyz * 2.0 - vec3(1.0);
    normalMap = normalize(normalMap);
    N = normalize(mat3(T, B, N) * normalMap);
  }

  outDiffuse = vec4(diffuse, 1.0);
  outSpecular = vec4(specular, clamp(shininess / 128.0, 0.0, 1.0));
  outNormal = vec4(N * 0.5 + vec3(0.5), 1.0);
  outPosition = vec4(positionViewSpace, 1.0);
}
