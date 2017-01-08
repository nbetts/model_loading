/**
 * [Program description]
 */

#ifndef MODEL_HEADER
#define MODEL_HEADER

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <vector>

#include "helpers.hpp"
#include "mesh.cpp"
#include "shader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb_image.h"

class Model
{
  public:
    Model(std::string modelFilepath = "");
    GLvoid load();
    GLvoid unload();
    GLvoid draw(Shader shader, GLuint isCullingEnabled);

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
};
  
#endif
