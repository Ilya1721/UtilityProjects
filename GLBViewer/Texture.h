#pragma once

namespace GLBViewer
{
  class Texture
  {
   public:
    Texture();
    ~Texture();
    unsigned int getId() const;

   protected:
    unsigned int mTexture {};
  };
}
