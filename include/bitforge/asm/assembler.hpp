#pragma once

#include <string>
#include <unordered_map>
#include <vector>

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
  bool isDecimal(const std::string &payload);
  // Pass 1: record (LABEL) -> ROM address. Pass 2 helper: predefined,
  // label, or fresh variable (allocated from 16 upward).
  void firstPass(const std::vector<std::string> &lines);
  int resolveSymbol(const std::string &name);
  bool assemble(std::vector<std::string> &lines, std::vector<std::string> &out);

private:
  void initPredefined();
  std::unordered_map<std::string, int> symbols_;
  int nextVar_ = 16;
};
} // namespace bitforge::assembler
