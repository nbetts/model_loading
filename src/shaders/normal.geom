#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

struct Material {
  sampler2D diffuse1;
};

in Data {
  vec3 normal;
  vec4 vNormal;
} vertices[];

out Data {
  vec4 colour;
} fragment;

uniform float normalLength;

void main()
{
  for (int i = 0; i < gl_in.length(); i++) {
    vec4 normalColour = vec4(vertices[i].normal * 2.0f, 1.0f);

    gl_Position = gl_in[i].gl_Position;
    normalColour.a = 1.0f;
    fragment.colour = normalColour;
    EmitVertex();

    gl_Position = gl_in[i].gl_Position + (vertices[i].vNormal * normalLength);
    normalColour.a = 0.0f;
    fragment.colour = normalColour;
    EmitVertex();

    EndPrimitive();
  }

}