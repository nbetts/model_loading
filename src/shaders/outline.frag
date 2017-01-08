#version 330 core

uniform vec4 outlineColour;

out vec4 colour;

void main()
{
  colour = outlineColour;
}