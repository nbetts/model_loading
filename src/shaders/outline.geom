#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in Data {
  vec4 normal;
} vertices[];

uniform float outlineSize;

void main()
{
  for (int i = 0; i < gl_in.length(); i++) {
    gl_Position = gl_in[i].gl_Position + (vertices[i].normal * outlineSize);
    EmitVertex();
  }

  EndPrimitive();
}