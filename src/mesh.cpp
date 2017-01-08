/**
 * [Program description]
 */

#include "mesh.hpp"

/**
 * Constructor to create and set the attributes of the mesh.
 */
Mesh::Mesh(std::vector<Vertex> meshVertices,
           std::vector<GLuint> meshIndices,
           std::vector<Texture> meshTextures)
{
  vertices = meshVertices;
  indices = meshIndices;
  textures = meshTextures;
}

GLvoid Mesh::load()
{
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), 
               &vertices[0], GL_STATIC_DRAW);  

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), 
               &indices[0], GL_STATIC_DRAW);

  // Vertex positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid*)offsetof(Vertex, position));
  // Normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid*)offsetof(Vertex, normal));
  // Texture coordinates
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid*)offsetof(Vertex, textureCoords));

  glBindVertexArray(0);
}

GLvoid Mesh::unload()
{
  glBindVertexArray(vao);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  glBindVertexArray(0);

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
}

GLvoid Mesh::draw(Shader shader)
{
  GLuint diffuseNo = 0, specularNo = 0;

  // Bind the mesh texture(s).
  for (GLuint i = 0; i < textures.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);

    std::string name = textures[i].type;
    std::string number = (name == "diffuse") ?
                         std::to_string(diffuseNo++) :
                         std::to_string(specularNo++);

    glUniform1i(glGetUniformLocation(shader.id,
                ("material." + name + number).c_str()), i); 
    glBindTexture(GL_TEXTURE_2D, textures[i].id);
  }
  glActiveTexture(GL_TEXTURE0);

  // Draw the mesh.
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}