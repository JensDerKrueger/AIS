#version 410 core

in vec3 posViewSpace;
in vec3 normalViewSpaceInterpolated;

uniform vec4 lightPosition;

uniform vec3 ka = vec3(0.05f, 0.05f, 0.05f); // material ambient color
uniform vec3 kd = vec3(0.0f, 0.0f, 0.8f); // material diffuse color
uniform vec3 ks = vec3(1.0f, 1.0f, 1.0f); // material specular color
uniform float shininess = 50.0f;

uniform vec3 la = vec3(0.9f, 0.9f, 0.9f); // light ambient color
uniform vec3 ld = vec3(0.9f, 0.9f, 0.9f); // light diffuse color
uniform vec3 ls = vec3(0.9f, 0.9f, 0.9f); // light specular color

out vec4 color;

void main() {
  vec3 normalViewSpace = normalize(normalViewSpaceInterpolated);
  vec3 lightVec = normalize(lightPosition.xyz - posViewSpace);

  // ambient color
  vec3 ambient = ka * la;

  // diffuse color
  float d = max(0, dot(normalViewSpace, lightVec));
  vec3 diffuse = d * kd * ld;

  // specular color
  vec3 viewVec =  normalize(-posViewSpace);
  vec3 reflected =  reflect(-lightVec, normalViewSpace);
  float s = pow(max(0, dot(viewVec, reflected)), shininess);
  vec3 specular = s * ks * ls;

  color = vec4(ambient + diffuse + specular, 1);
}
