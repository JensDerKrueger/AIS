#version 410 core

uniform sampler2D td;

in vec2 tc;

out vec4 color;

void main()
{
  color = texture(td, tc);
}
