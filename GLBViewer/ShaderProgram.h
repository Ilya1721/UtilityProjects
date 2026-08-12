#pragma once

#include <string>

namespace GLBViewer
{
  class ShaderObject
  {
   public:
    ~ShaderObject();
    int getUniformLocation(const char* locationName) const;
    void bind() const;
    void load(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

   private:
    int loadShader(const std::string& shaderPath, int shaderType) const;

   private:
    int mVertexShader {};
    int mFragmentShader {};
    int mShaderProgram {};
  };

  class ShaderProgram
  {
   public:
    ShaderProgram(
      const std::string& vertexShaderPath, const std::string& fragmentShaderPath
    );
    void bind() const;

   protected:
    int getUniformLocation(const char* locationName) const;

   private:
    ShaderObject mShaderObject;
  };
}
