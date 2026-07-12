in vec3 posViewSpaceInterpolated;
in vec3 normalViewSpaceInterpolated;
in vec3 tangentViewSpaceInterpolated;
in vec3 binormViewSpaceInterpolated;
in vec2 texCoordsInterpolated;
in vec4 shadowPos;

uniform sampler2D tn;
uniform sampler2DShadow shadowMap;

uniform vec4 lightPosition;

out vec4 color;

void main() {
  float depthBias = 0.01;

  vec3 ka = vec3(0.05f, 0.05f, 0.05f); // material ambient color
  vec3 kd = vec3(0.0f, 0.0f, 0.8f); // material diffuse color
  vec3 ks = vec3(1.0f, 1.0f, 1.0f); // material specular color
  float shininess = 50.0f;

  vec3 la = vec3(0.9f, 0.9f, 0.9f); // light ambient color
  vec3 ld = vec3(0.9f, 0.9f, 0.9f); // light diffuse color
  vec3 ls = vec3(0.9f, 0.9f, 0.9f); // light specular color

  vec3 normalMap = texture(tn, texCoordsInterpolated).xyz;

  vec3 N = normalize(normalViewSpaceInterpolated);
  vec3 T = normalize(tangentViewSpaceInterpolated);
  vec3 B = normalize(binormViewSpaceInterpolated);

  if(normalMap != vec3(0.0, 0.0, 0.0)) {
    normalMap = 2.0 * (normalMap - vec3(0.5)); // [0, 1] should map to [-1, 1]
    normalMap = normalize(normalMap);

    mat3 tbnMatrix = mat3(T, B, N);
    N = tbnMatrix * normalMap;
    N = normalize(N);
  }


  vec3 lightVec = normalize(lightPosition.xyz - posViewSpaceInterpolated);

  // ambient color
  vec3 ambient = ka * la;

  // diffuse color
  float d = max(0.0, dot(N, lightVec));
  vec3 diffuse = d * kd * ld;

  float s = 0.0;
  if(d > 0.0) {
    vec3 viewVec =  normalize(-posViewSpaceInterpolated); // camera is placed in origin in view space, view vector == -posViewSpace
    vec3 reflected =  reflect(-lightVec, N); // reflect expects L pointing to surface
    s = pow(max(0.0, dot(viewVec, reflected)), shininess);
  }

  vec3 specular = s * ks * ls;

  vec4 biasedShadow = shadowPos;
  biasedShadow.z -= depthBias;
  float shadowPercentage = textureProj(shadowMap,biasedShadow);

  vec4 lightColor = vec4(ambient + diffuse + specular, 1.0);
  vec4 shadowColor = vec4(ambient, 1.0);

  color = mix(shadowColor, lightColor, shadowPercentage);

}
