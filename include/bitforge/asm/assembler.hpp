#pragma once

#include <string>

namespace bitforge::assembler {
enum class InstructionType {
  A_INSTRUCTION,
  C_INSTRUCTION,
  L_INSTRUCTION,
  ERROR
};

struct C_split {
  std::string dest;
  std::string comp;
  std::string jump;
};
class Assembler {
public:
  Assembler();
  std::string cleanLine(std::string line);
  InstructionType classify(std::string line);
  C_split C_Splitter(std::string line);
  // Mnemonic -> bit strings. Unknown mnemonic returns "" (error signal),
  // except empty dest/jump which validly mean "000".
  std::string compBits(const std::string &m);
  std::string destBits(const std::string &m);
  std::string jumpBits(const std::string &m);
};
} // namespace bitforge::assembler
