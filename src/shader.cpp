/**
 * [Program description]
 */

#include "shader.hpp"

Shader::Shader(std::string vertexFile, std::string fragmentFile,
               std::string geometryFile)
{
  vertexShaderFile = vertexFile;
  geometryShaderFile = geometryFile;
  fragmentShaderFile = fragmentFile;
}

/**
 * Read and create the shader. This involves retrieving the
 * shader source code from the given vertex/geometry/fragment files and
 * compiling the code, then linking them into a shader program.
 */
GLvoid Shader::load()
{
  GLint compileStatus;
  GLchar compileLog[LOG_MSG_LENGTH];
  GLuint vertexShaderID, fragmentShaderID, geometryShaderID = 0;
  GLuint isGeometryShaderIncluded = !geometryShaderFile.empty();

  // Vertex shader
  const GLchar* vertexShaderSource = readFile(vertexShaderFile);
  vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShaderID, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShaderID);
  glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &compileStatus);

  if (compileStatus != GL_TRUE) {
    glGetShaderInfoLog(vertexShaderID, LOG_MSG_LENGTH, NULL, compileLog);
    fprintf(stderr, "\nVertex compilation error (ID: %d) in file: %s\n%s\n",
            vertexShaderID, vertexShaderFile.c_str(), compileLog);

    exit(EXIT_FAILURE);
  }

  // Fragment shader
  const GLchar* fragmentShaderSource = readFile(fragmentShaderFile);
  fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShaderID, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShaderID);
  glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &compileStatus);

  if (compileStatus != GL_TRUE) {
    glGetShaderInfoLog(fragmentShaderID, LOG_MSG_LENGTH, NULL, compileLog);
    fprintf(stderr, "\nFragment compilation error (ID: %d) in file: %s\n%s\n",
            fragmentShaderID, fragmentShaderFile.c_str(), compileLog);

    exit(EXIT_FAILURE);
  }

  // Geometry shader
  if (isGeometryShaderIncluded) {
    const GLchar* geometryShaderSource = readFile(geometryShaderFile);
    geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometryShaderID, 1, &geometryShaderSource, NULL);
    glCompileShader(geometryShaderID);
    glGetShaderiv(geometryShaderID, GL_COMPILE_STATUS, &compileStatus);

    if (compileStatus != GL_TRUE) {
      glGetShaderInfoLog(geometryShaderID, LOG_MSG_LENGTH, NULL, compileLog);
      fprintf(stderr, "\nGeometry compilation error (ID: %d) in file: %s\n%s\n",
              geometryShaderID, geometryShaderFile.c_str(), compileLog);

      exit(EXIT_FAILURE);
    }
  }

  // Create and compile the shader program.
  id = glCreateProgram();
  glAttachShader(id, vertexShaderID);
  glAttachShader(id, fragmentShaderID);

  if (isGeometryShaderIncluded) {
    glAttachShader(id, geometryShaderID);
  }

  glLinkProgram(id);
  glGetProgramiv(id, GL_LINK_STATUS, &compileStatus);

  if (compileStatus != GL_TRUE) {
    glGetProgramInfoLog(id, LOG_MSG_LENGTH, NULL, compileLog);
    fprintf(stderr, "\nShader program compilation error (ID: %d):\n%s\n",
           id, compileLog);

    exit(EXIT_FAILURE);
  }

  // Delete the shaders as they are now linked to the program.
  glDetachShader(id, vertexShaderID);
  glDetachShader(id, fragmentShaderID);
  glDeleteShader(vertexShaderID);
  glDeleteShader(fragmentShaderID);

  if (isGeometryShaderIncluded) {
    glDetachShader(id, geometryShaderID);
    glDeleteShader(geometryShaderID);
  }
}

GLvoid Shader::unload()
{
  glDeleteProgram(id);
}

GLvoid Shader::use()
{
  glUseProgram(id);
}