#version 460 core

in VertexData
{
  vec3 position;
  vec3 normal;
  vec2 uv;
  vec4 color;
} vertexData;

out vec4 FragColor;
  
uniform sampler2D screenTexture;
uniform float time;

void main()
{ 
  // vec2 uv = vertexData.uv;
  vec2 uv = vertexData.uv - 0.5;
  float a = atan(uv.y, uv.x);
  float r = length(uv);
  // vec2 st = vec2(a / 3.1415, 0.1 / r);
  vec2 st = vec2(a / 3.1415, 0.1 / r) + 0.2 * time;

  vec3 col = texture(screenTexture, st).rgb;
  col *= 3.5 * r;
  FragColor = vec4(col, 1);
}