#pragma once

#include <functional>
#include <memory>
#include <string>

#include "CubemapTexture.h"
#include "RenderBuffer.h"
#include "Texture2D.h"

namespace GLBViewer
{
  class IBL
  {
   public:
    IBL(const std::string& hdriFilePath);

    std::unique_ptr<CubemapTexture> loadIrradianceMap() const;
    std::unique_ptr<CubemapTexture> loadPrefilteredEnvMap() const;
    std::unique_ptr<Texture2D> loadBRDFLUT() const;

   private:
    void renderToCubemap(
      const CubemapTexture& cubemap,
      int viewportSize,
      const std::function<void(const glm::mat4& view)>& viewSetter
    ) const;
    std::unique_ptr<CubemapTexture> loadEnvMap(const Texture2D& hdrTexture) const;

   private:
    std::unique_ptr<CubemapTexture> mEnvMap;
    std::unique_ptr<CubeRenderBuffer> mCubeRenderBuffer;
  };
}
