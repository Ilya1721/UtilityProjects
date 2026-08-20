#pragma once

namespace GLBViewer
{
  inline constexpr float CUBE_VERTICES[] = {
    // back face
    -1, -1, -1, 1, 1, -1, 1, -1, -1, 1, 1, -1, -1, -1, -1, -1, 1, -1,
    // front face
    -1, -1, 1, 1, -1, 1, 1, 1, 1, 1, 1, 1, -1, 1, 1, -1, -1, 1,
    // left face
    -1, 1, 1, -1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, -1, 1, 1,
    // right face
    1, 1, 1, 1, -1, -1, 1, 1, -1, 1, -1, -1, 1, 1, 1, 1, -1, 1,
    // bottom face
    -1, -1, -1, 1, -1, -1, 1, -1, 1, 1, -1, 1, -1, -1, 1, -1, -1, -1,
    // top face
    -1, 1, -1, 1, 1, 1, 1, 1, -1, 1, 1, 1, -1, 1, -1, -1, 1, 1
  };
  constexpr float SQUARE_VERTICES[] = {
    // bottom-left   bottom-right   top-right
    -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
    // bottom-left   top-right      top-left
    -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f
  };
}
