/**
 * [Program description]
 */

#ifndef MESH_HEADER
#define MESH_HEADER

#include <glm/glm.hpp>
#include <vector>
#include "shader.hpp"

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 textureCoords;
};

struct Texture {
  GLuint id;
  std::string type;
  aiString filepath;
};

class Mesh
{
  public:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> meshVertices,
         std::vector<GLuint> meshIndices,
         std::vector<Texture> meshTextures);
    GLvoid load();
    GLvoid unload();
    GLvoid draw(Shader shader);

  private:
    GLuint vao, vbo, ebo;
};
  
#endif
