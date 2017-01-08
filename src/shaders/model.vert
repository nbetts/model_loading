#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textureCoords;

out Data {
  vec4 position;
  vec3 normal;
  vec2 textureCoords;
  vec4 vNormal; // temp
} vertex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  gl_Position = projection * view * model * vec4(position, 1.0f);

  vertex.position = model * vec4(position, 1.0f);
  vertex.normal = normal;
  vertex.textureCoords = textureCoords;
  
  vertex.vNormal = normalize(projection * vec4(mat3(
                   transpose(inverse(view * model))) * normal, 1.0f));
}