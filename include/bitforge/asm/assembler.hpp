#pragma once

#include <string>

namespace bitforge::assembler {
enum class InstructionType {
  A_INSTRUCTION,
  C_INSTRUCTION,
  L_INSTRUCTION,
  ERROR
};
class Assembler {
public:
  Assembler();
  std::string cleanLine(std::string line);
  InstructionType classify(std::string line);
};
} // namespace bitforge::assembler
