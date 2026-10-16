#pragma once

#include <string>

namespace bitforge::assembler {
class Assembler {
public:
  Assembler();
  std::string cleanLine(std::string line);
};
} // namespace bitforge::assembler
