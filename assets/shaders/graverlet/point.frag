#version 460 core

in VertexData
{
  vec3 position;
  vec3 normal;
  vec2 uv;
  vec4 color;
} vertexData;

out vec4 FragColor;

void main()
{
  vec2 p = 2 * gl_PointCoord - 1;
  // if (dot(p, p) > 1)
    // discard;
  // FragColor = vec4(1, 1, 1, 1);
  float alpha = (1.0 - smoothstep(0.25, 1.0, length(p))) * 0.1;
  FragColor = vec4(vertexData.color.rgb, alpha);
}