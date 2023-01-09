#version 410 core

in vec3 posViewSpaceInterpolated;
in vec3 normalViewSpaceInterpolated;
in vec3 tangentViewSpaceInterpolated;
in vec3 binormViewSpaceInterpolated;
in vec2 texCoordsInterpolated;
in vec4 shadowPos;

uniform sampler2D td;
uniform sampler2D ts;
uniform sampler2D tn;
uniform sampler2DShadow shadowMap;
uniform float depthBias = 0.01;

uniform vec4 lightPosition;

uniform vec3 ka = vec3(0.05f, 0.05f, 0.05f); // material ambient color
uniform float shininess = 50.0f;

uniform vec3 la = vec3(0.9f, 0.9f, 0.9f); // light ambient color
uniform vec3 ld = vec3(0.9f, 0.9f, 0.9f); // light diffuse color
uniform vec3 ls = vec3(0.9f, 0.9f, 0.9f); // light specular color

out vec4 color;

void main() {
  vec3 kd = texture(td, texCoordsInterpolated).rgb;
  vec3 ks = texture(ts, texCoordsInterpolated).rgb;
  vec3 normalMap = texture(tn, texCoordsInterpolated).xyz;

  vec3 N = normalize(normalViewSpaceInterpolated);
  vec3 T = normalize(tangentViewSpaceInterpolated);
  vec3 B = normalize(binormViewSpaceInterpolated);

  if(normalMap != vec3(0, 0, 0)) {
    normalMap = 2 * (normalMap - vec3(0.5)); // [0, 1] should map to [-1, 1]
    normalMap = normalize(normalMap);

    mat3 tbnMatrix = mat3(T, B, N);
    N = tbnMatrix * normalMap;
    N = normalize(N);
  }


  vec3 lightVec = normalize(lightPosition.xyz - posViewSpaceInterpolated);

  // ambient color
  vec3 ambient = ka * la;

  // diffuse color
  float d = max(0, dot(N, lightVec));
  vec3 diffuse = d * kd * ld;

  float s = 0;
  if(d > 0) {
    vec3 viewVec =  normalize(-posViewSpaceInterpolated); // camera is placed in origin in view space, view vector == -posViewSpace
    vec3 reflected =  reflect(-lightVec, N); // reflect expects L pointing to surface
    s = pow(max(0, dot(viewVec, reflected)), shininess);
  }

  vec3 specular = s * ks * ls;

  vec4 biasedShadow = shadowPos;
  biasedShadow.z -= depthBias;
  float shadowPercentage = textureProj(shadowMap,biasedShadow);

  vec4 lightColor = vec4(ambient + diffuse + specular, 1);
  vec4 shadowColor = vec4(ambient, 1);

  color = mix(shadowColor, lightColor, shadowPercentage);
}
