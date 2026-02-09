in vec3 vertexPosition;  // vertex position in object/model space
in vec3 vertexNormal;    // vertex normal in object/model space

uniform mat4 MVP; // model-view-projection Matrix

uniform vec3 kd; // material diffuse color

out vec4 C;

void main()
{
	gl_Position = MVP * vec4(vertexPosition, 1.0);
	C = vec4(0.5 * vertexNormal + 0.5 * kd, 1.0);
}
