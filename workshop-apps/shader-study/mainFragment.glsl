#version 460 core

in VertexData
{
  vec3 position;
  vec3 normal;
  vec2 uv;
  vec4 color;
} vertexData;

uniform vec2 RenderTargetSize;

out vec4 FragColor;

void main()
{
  FragColor = vec4(gl_FragCoord.x / RenderTargetSize.x, 0, gl_FragCoord.y / RenderTargetSize.y, 1);
}