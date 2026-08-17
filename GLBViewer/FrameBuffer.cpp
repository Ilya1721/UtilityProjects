#include "FrameBuffer.h"

#ifdef __gl_h_
#undef __gl_h_
#endif
#include <glad/glad.h>

namespace GLBViewer
{
  FrameBuffer::FrameBuffer(int width, int height)
  {
    glGenFramebuffers(1, &mFBO);
    glGenRenderbuffers(1, &mRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRBO);
  }

  FrameBuffer::~FrameBuffer()
  {
    glDeleteFramebuffers(1, &mFBO);
    glDeleteFramebuffers(1, &mRBO);
  }

  void FrameBuffer::bind() const
  {
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, mRBO);
  }
}
