#include "FileUtils.h"

#include <fstream>
#include <sstream>

namespace GLBViewer
{
  std::string readFile(const std::string& filePath)
  {
    std::ifstream file(filePath);
    if (!file.is_open())
    {
      auto excMsg = "Could not open the file -> " + filePath;
      throw std::exception(excMsg.c_str());
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
  }
}
