#include "ShaderProgram.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

#include "FileUtils.h"

namespace GLBViewer
{
  ShaderObject::~ShaderObject()
  {
    glDeleteShader(mVertexShader);
    glDeleteShader(mFragmentShader);
    glDeleteProgram(mShaderProgram);
  }

  int ShaderObject::getUniformLocation(const char* locationName) const
  {
    return glGetUniformLocation(mShaderProgram, locationName);
  }

  void ShaderObject::bind() const
  {
    glUseProgram(mShaderProgram);
  }

  void ShaderObject::load(
    const std::string& vertexShaderPath, const std::string& fragmentShaderPath
  )
  {
    mVertexShader = loadShader(vertexShaderPath, GL_VERTEX_SHADER);
    mFragmentShader = loadShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
    mShaderProgram = glCreateProgram();
    glAttachShader(mShaderProgram, mVertexShader);
    glAttachShader(mShaderProgram, mFragmentShader);
    glLinkProgram(mShaderProgram);
    GLint linkStatus;
    glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE)
    {
      throw std::exception("Could not link the shader program");
    }
  }

  int ShaderObject::loadShader(const std::string& shaderPath, int shaderType) const
  {
    auto shader = glCreateShader(shaderType);
    auto shaderCode = readFile(shaderPath);
    auto stringArr = {shaderCode.c_str()};
    auto lengthArr = {static_cast<int>(shaderCode.length())};
    glShaderSource(shader, 1, stringArr.begin(), lengthArr.begin());
    glCompileShader(shader);
    GLint compileStatus = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE)
    {
      auto excMsg = "Could not compile shader -> " + shaderPath;
      throw std::exception(excMsg.c_str());
    }
    return shader;
  }

  ShaderProgram::ShaderProgram(
    const std::string& vertexShaderPath, const std::string& fragmentShaderPath
  )
  {
    mShaderObject.load(vertexShaderPath, fragmentShaderPath);
  }

  int ShaderProgram::getUniformLocation(const char* locationName) const
  {
    return mShaderObject.getUniformLocation(locationName);
  }

  void ShaderProgram::bind() const
  {
    mShaderObject.bind();
  }
}
