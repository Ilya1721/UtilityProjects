#pragma once

namespace GLBViewer
{
  class Texture2D
  {
   public:
    Texture2D(int width, int height) noexcept;
    ~Texture2D();
    int getId() const;
    int getWidth() const;
    int getHeight() const;
    void bind() const;

   private:
    int mWidth {};
    int mHeight {};
    unsigned int mTexture {};
  };
}
