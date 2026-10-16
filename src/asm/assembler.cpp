#include "bitforge/asm/assembler.hpp"
#include <cstddef>
#include <string>
namespace bitforge::assembler {

Assembler::Assembler() {};
std::string Assembler::cleanLine(std::string line) {
  if (line == "")
    return "";
  std::string new_line = "";
  for (std::size_t ch = 0; ch < line.size(); ch++) {

    if (line[ch] == ' ' || line[ch] == '\t' || line[ch] == '\r')
      continue;

    if (line[ch] == '/') {
      if (ch < line.size() - 1 && line[ch + 1] == '/')
        break;
    }

    new_line += line[ch];
  }

  return new_line;
}

InstructionType Assembler::classify(std::string line) {
  if (line.empty())
    return InstructionType::ERROR;
  if (line == "@")
    return InstructionType::ERROR;
  if (line[0] == '@')
    return InstructionType::A_INSTRUCTION;
  if (line[0] == '(') {
    size_t n = line.size();
    if (n < 3)
      return InstructionType::ERROR;
    return (line[n - 1] == ')') ? InstructionType::L_INSTRUCTION
                                : InstructionType::ERROR;
  }

  return InstructionType::C_INSTRUCTION;
}
} // namespace bitforge::assembler
