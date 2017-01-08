/**
 * [Program description]
 */

#ifndef MODEL_HEADER
#define MODEL_HEADER

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <limits>
#include <vector>

#include "helpers.hpp"
#include "mesh.cpp"
#include "shader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb_image.h"

#define X 0
#define Y 1
#define Z 2

class Model
{
  public:
    GLfloat minX, maxX, minY, maxY, minZ, maxZ;
    glm::vec3 centerPosition;

    Model(std::string modelFilepath = "");
    GLvoid load();
    GLvoid unload();
    GLvoid draw(Shader shader, GLuint isCullingEnabled);
    GLvoid normalize(GLfloat min, GLfloat max);
    GLvoid printBoundingBox();

  private:
    std::vector<Mesh> meshes;
    std::vector<Texture> loadedTextures;
    std::string filepath;
    std::string directory;

    GLvoid processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* material,
                                              aiTextureType type,
                                              std::string typeName);
    GLuint loadTexture(const GLchar* filepath, std::string directory);
    GLvoid calculateBoundingBox();
};
  
#endif
