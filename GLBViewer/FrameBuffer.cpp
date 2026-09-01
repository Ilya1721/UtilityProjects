#include "FrameBuffer.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

namespace GLBViewer
{
  FrameBuffer::FrameBuffer()
  {
    glGenFramebuffers(1, &mFBO);
    glGenRenderbuffers(1, &mRBO);
  }

  FrameBuffer::~FrameBuffer()
  {
    glDeleteFramebuffers(1, &mFBO);
    glDeleteFramebuffers(1, &mRBO);
  }

  void FrameBuffer::addColorAttachment(const Texture2D& texture, int mipLevel) const
  {
    bind();
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.getId(), mipLevel
    );
  }

  void FrameBuffer::addColorAttachment(
    const CubemapTexture& texture, int faceIdx, int mipLevel
  ) const
  {
    bind();
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIdx,
      texture.getId(), mipLevel
    );
  }

  void FrameBuffer::addDepthAttachment(int width, int height) const
  {
    bind();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRBO);
  }

  void FrameBuffer::addDepthAttachment(const Texture2D& texture, int mipLevel) const
  {
    bind();
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture.getId(), mipLevel
    );
  }

  void FrameBuffer::bind() const
  {
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, mRBO);
  }

  void FrameBuffer::copyPixels(int width, int height, const FrameBuffer& target, int mask)
    const
  {
    copyPixels(width, height, target.mFBO, mask);
  }

  void FrameBuffer::copyPixels(int width, int height, int targetFBO, int mask) const
  {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFBO);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, mask, GL_NEAREST);
  }

  void FrameBuffer::resizeDepthAttachment(int width, int height) const
  {
    bind();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
  }

  void FrameBuffer::enableColorDraw(bool enable) const
  {
    bind();
    auto state = enable ? GL_COLOR_ATTACHMENT0 : GL_NONE;
    glDrawBuffer(state);
  }
}
