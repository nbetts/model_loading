/**
 * [Program description]
 */

#include "model.hpp"

/**
 * Constructor to create and set the attributes of the model.
 */
Model::Model(std::string modelFilepath)
{
  if (!modelFilepath.empty()) {
    filepath = modelFilepath;
    directory = filepath.substr(0, filepath.find_last_of('/'));
  }

  minX = minY = minZ = maxX = maxY = maxZ = 0.0f;
  centerPosition = glm::vec3(0.0f);
}

GLvoid Model::load()
{
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(filepath,
                         aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!scene || !scene->mRootNode ||
      scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE) {
    fprintf(stderr, "\nLoad model error in file: %s\n%s\n",
            filepath.c_str(), importer.GetErrorString());

    exit(EXIT_FAILURE);
  }

  processNode(scene->mRootNode, scene);
  calculateBoundingBox();
}

/**
 * Clear all the buffers used by each mesh in the model and free the memory used
 * by each mesh and loaded texture.
 */
GLvoid Model::unload()
{
  for (GLuint i = 0; i < meshes.size(); i++) {
    meshes[i].unload();
  }

  meshes.clear();
  loadedTextures.clear();
}

GLvoid Model::draw(Shader shader, GLuint isCullingEnabled)
{
  if (isCullingEnabled) {
    glEnable(GL_CULL_FACE);
  }

  for (GLuint i = 0; i < meshes.size(); i++) {
    meshes[i].draw(shader);
  }

  if (isCullingEnabled) {
    glDisable(GL_CULL_FACE);
  }
}

/**
 * Normalize the model's vertex positions in between min and max.
 */
GLvoid Model::normalize(GLfloat min, GLfloat max)
{
  GLfloat x, y, z;
  GLfloat scaleFactor;

  GLfloat size = max - min;
  GLfloat magX = fabsf(maxX - minX);
  GLfloat magY = fabsf(maxY - minY);
  GLfloat magZ = fabsf(maxZ - minZ);

  GLuint largestDimension = magY > magX ? magZ > magY ? Z : Y : X;

  switch (largestDimension) {
    case X: scaleFactor = 1.0f / magX; break;
    case Y: scaleFactor = 1.0f / magY; break;
    case Z: scaleFactor = 1.0f / magZ; break;
  }

  for (GLuint i = 0; i < meshes.size(); i++) {
    for (GLuint j = 0; j < meshes[i].vertices.size(); j++) {
      x = meshes[i].vertices[j].position.x;
      y = meshes[i].vertices[j].position.y;
      z = meshes[i].vertices[j].position.z;

      switch (largestDimension) {
        case X:
          x = size * (x - minX) / (maxX - minX) + min;
          y = size * (y * scaleFactor);
          z = size * (z * scaleFactor);
          break;
        case Y:
          x = size * (x * scaleFactor);
          y = size * (y - minY) / (maxY - minY) + min;
          z = size * (z * scaleFactor);
          break;
        case Z:
          x = size * (x * scaleFactor);
          y = size * (y * scaleFactor);
          z = size * (z - minZ) / (maxZ - minZ) + min;
          break;
      }

      meshes[i].vertices[j].position.x = x;
      meshes[i].vertices[j].position.y = y;
      meshes[i].vertices[j].position.z = z;
    }

    meshes[i].reload();
  }

  calculateBoundingBox();
}

GLvoid Model::processNode(aiNode* node, const aiScene* scene)
{
  // Process the meshes of this node.
  for (GLuint i = 0; i < node->mNumMeshes; i++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]]; 
    meshes.push_back(processMesh(mesh, scene));     
  }

  // Process thes meshes of this node's children.
  for (GLuint i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  std::vector<Texture> textures;
  glm::vec3 vector;

  // Process vertex positions, normals and texture coordinates
  for (GLuint i = 0; i < mesh->mNumVertices; i++)
  {
      Vertex vertex;

      vector.x = mesh->mVertices[i].x;
      vector.y = mesh->mVertices[i].y;
      vector.z = mesh->mVertices[i].z; 
      vertex.position = vector;

      vector.x = mesh->mNormals[i].x;
      vector.y = mesh->mNormals[i].y;
      vector.z = mesh->mNormals[i].z;
      vertex.normal = vector;

      if(mesh->mTextureCoords[0]) {
        vector.x = mesh->mTextureCoords[0][i].x; 
        vector.y = mesh->mTextureCoords[0][i].y;
      }
      else {
        vector.x = 0.0f;
        vector.y = 0.0f;
      }
      vertex.textureCoords = glm::vec2(vector);

      vertices.push_back(vertex);
  }

  // Process indices
  for (GLuint i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];

    for (GLuint j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  // Process material
  if (mesh->mMaterialIndex > 0) {
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<Texture> diffuseMaps = loadMaterialTextures(material, 
                                    aiTextureType_DIFFUSE, "diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    std::vector<Texture> specularMaps = loadMaterialTextures(material, 
                                    aiTextureType_SPECULAR, "specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
  }

  Mesh newMesh(vertices, indices, textures);
  newMesh.load();

  return newMesh;
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* material,
                                                 aiTextureType type,
                                                 std::string typeName)
{
  std::vector<Texture> textures;
  Texture texture;
  aiString filepath;
  GLuint isTextureAlreadyLoaded;

  for (GLuint i = 0; i < material->GetTextureCount(type); i++) {
    material->GetTexture(type, i, &filepath);
    isTextureAlreadyLoaded = false;

    // Check if the texture has already been loaded.
    for (GLuint j = 0; j < loadedTextures.size(); j++) {
      if (filepath == loadedTextures[j].filepath) {
        textures.push_back(loadedTextures[j]);
        isTextureAlreadyLoaded = true; 
        break;
      }
    }

    // Otherwise load the texture.
    if (!isTextureAlreadyLoaded) {
      texture.id = loadTexture(filepath.C_Str(), directory);
      texture.type = typeName;
      texture.filepath = filepath;
      textures.push_back(texture);
      loadedTextures.push_back(texture);
    }
  }

  return textures;
}

GLuint Model::loadTexture(const GLchar* filepath, std::string directory)
{
  std::string filename(filepath);
  filename = directory + '/' + filename;

  GLuint textureID;
  glGenTextures(1, &textureID);

  GLint width, height;
  GLubyte* image = stbi_load(filename.c_str(), &width, &height, 0, STBI_rgb_alpha);

  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(image);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  return textureID;
}

GLvoid Model::calculateBoundingBox()
{
  GLfloat maxFloatValue = std::numeric_limits<float>::max();
  glm::vec3 vector;

  minX = minY = minZ = maxFloatValue;
  maxX = maxY = maxZ = -maxFloatValue;

  for (GLuint i = 0; i < meshes.size(); i++) {
    for (GLuint j = 0; j < meshes[i].vertices.size(); j++) {
      vector = meshes[i].vertices[j].position;

      if (vector.x < minX) {
        minX = vector.x;
      } else if (vector.x > maxX) {
        maxX = vector.x;
      }

      if (vector.y < minY) {
        minY = vector.y;
      } else if (vector.y > maxY) {
        maxY = vector.y;
      }

      if (vector.z < minZ) {
        minZ = vector.z;
      } else if (vector.z > maxZ) {
        maxZ = vector.z;
      }
    }
  }

  centerPosition = glm::vec3(average({minX, maxX}),
                             average({minY, maxY}),
                             average({minZ, maxZ}));
}

GLvoid Model::printBoundingBox()
{
  printf("Bounding box for %s\n", filepath.c_str());
  printf("min = (%.3f, %.3f, %.3f)\n", minX, minY, minZ);
  printf("max = (%.3f, %.3f, %.3f)\n", maxX, maxY, maxZ);
  printf("center = (%.3f, %.3f, %.3f)\n", centerPosition.x,
                                          centerPosition.y,
                                          centerPosition.z);
}