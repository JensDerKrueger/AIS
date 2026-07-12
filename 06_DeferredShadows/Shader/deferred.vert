out vec2 texCoords;

void main() {
  vec2 positions[3] = vec2[3](
    vec2(-1.0, -1.0),
    vec2( 3.0, -1.0),
    vec2(-1.0,  3.0)
  );

  vec2 position = positions[gl_VertexID];
  gl_Position = vec4(position, 0.0, 1.0);
  texCoords = position * 0.5 + vec2(0.5);
}
