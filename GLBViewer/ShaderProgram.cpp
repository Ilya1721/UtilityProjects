#include "ShaderProgram.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

#include <unordered_set>

#include "FileUtils.h"

namespace
{
  using namespace GLBViewer;

  const std::string INCLUDE = "#include";

  std::string assembleShader(const std::string& shaderPath)
  {
    size_t lastIncludePos = std::string::npos;
    std::unordered_set<std::string> usedIncludePaths;
    auto shaderCode = readFile(shaderPath);
    while (true)
    {
      auto pos = shaderCode.find(INCLUDE);
      if (pos == std::string::npos)
      {
        break;
      }
      lastIncludePos = pos;
      shaderCode.erase(pos, INCLUDE.length() + 1);
      auto pathStart = shaderCode.find('\"', pos);
      auto pathEnd = shaderCode.find('\"', pathStart + 1);
      std::string includePath(shaderCode, pathStart + 1, pathEnd - pathStart - 1);
      shaderCode.erase(pathStart, includePath.length() + 2);
      if (usedIncludePaths.find(includePath) == usedIncludePaths.end())
      {
        auto includedCode = assembleShader(includePath);
        shaderCode.insert(lastIncludePos, includedCode);
        lastIncludePos += includedCode.length();
        usedIncludePaths.insert(includePath);
      }
    }
    return shaderCode;
  }

  int loadShader(const std::string& shaderPath, int shaderType)
  {
    auto shader = glCreateShader(shaderType);
    auto shaderCode = assembleShader(shaderPath);
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
}

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
