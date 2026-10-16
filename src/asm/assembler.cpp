#include "bitforge/asm/assembler.hpp"
#include <cstddef>
#include <string>
#include <unordered_map>
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

C_split Assembler::C_Splitter(std::string line) {
  std::string dest = "";
  std::string comp = "";
  std::string jump = "";
  size_t n = line.size();
  size_t i = 0;
  while (i < n && line[i] != '=') {
    dest += line[i++];
  }
  if (i == n) {
    dest = "";
    i = 0;
  } else {
    i++;
  }
  while (i < n && line[i] != ';') {
    comp += line[i++];
  }
  i++;
  while (i < n) {
    jump += line[i++];
  }

  return {dest, comp, jump};
}

namespace {
const std::unordered_map<std::string, std::string> kComp = {
    {"0", "0101010"}, {"1", "0111111"},   {"-1", "0111010"},
    {"D", "0001100"}, {"A", "0110000"},   {"!D", "0001101"},
    {"!A", "0110001"}, {"-D", "0001111"}, {"-A", "0110011"},
    {"D+1", "0011111"}, {"A+1", "0110111"}, {"D-1", "0001110"},
    {"A-1", "0110010"}, {"D+A", "0000010"}, {"D-A", "0010011"},
    {"A-D", "0000111"}, {"D&A", "0000000"}, {"D|A", "0010101"},
    {"M", "1110000"}, {"!M", "1110001"},   {"-M", "1110011"},
    {"M+1", "1110111"}, {"M-1", "1110010"}, {"D+M", "1000010"},
    {"D-M", "1010011"}, {"M-D", "1000111"}, {"D&M", "1000000"},
    {"D|M", "1010101"},
};
const std::unordered_map<std::string, std::string> kDest = {
    {"", "000"},   {"M", "001"},  {"D", "010"}, {"MD", "011"},
    {"A", "100"},  {"AM", "101"}, {"AD", "110"}, {"AMD", "111"},
};
const std::unordered_map<std::string, std::string> kJump = {
    {"", "000"},    {"JGT", "001"}, {"JEQ", "010"}, {"JGE", "011"},
    {"JLT", "100"}, {"JNE", "101"}, {"JLE", "110"}, {"JMP", "111"},
};
std::string Lookup(const std::unordered_map<std::string, std::string> &t,
                   const std::string &m) {
  auto it = t.find(m);
  return it == t.end() ? "" : it->second;
}
} // namespace

std::string Assembler::compBits(const std::string &m) {
  return Lookup(kComp, m);
}
std::string Assembler::destBits(const std::string &m) {
  return Lookup(kDest, m);
}
std::string Assembler::jumpBits(const std::string &m) {
  return Lookup(kJump, m);
}
} // namespace bitforge::assembler
