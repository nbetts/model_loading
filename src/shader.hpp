/**
 * [Program description]
 */

#ifndef SHADER_HEADER
#define SHADER_HEADER

#define LOG_MSG_LENGTH 256

class Shader
{
  public:
    GLuint id;

    Shader(std::string vertexFile = "", std::string fragmentFile = "",
           std::string geometryFile = "");
    GLvoid load();
    GLvoid unload();
    GLvoid use();

  private:
    std::string vertexShaderFile;
    std::string geometryShaderFile;
    std::string fragmentShaderFile;
};
  
#endif