#pragma once

#include <memory>
#include <string>

#include "Mesh.h"

std::vector<Vertex> read(const std::string& path);
void write(const std::vector<Vertex>& vertices, const std::string& path);
