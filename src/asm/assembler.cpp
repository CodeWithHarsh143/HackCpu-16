#include "bitforge/asm/assembler.hpp"
#include "bitforge/hardware/types.hpp"
#include <cstddef>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace bitforge::assembler {

hardware::Bits16 A_Instruction_in_bits(int addr) {
  hardware::Bits16 b;
  b[15] = 0;
  addr &= 0xFFFF;
  for (int i = 0; i < 15; i++)
    b[i] = (addr >> i) & 1;
  return b;
}

Assembler::Assembler() { initPredefined(); };

void Assembler::initPredefined() {
  for (int i = 0; i <= 15; i++)
    symbols_["R" + std::to_string(i)] = i;
  symbols_["SP"] = 0;
  symbols_["LCL"] = 1;
  symbols_["ARG"] = 2;
  symbols_["THIS"] = 3;
  symbols_["THAT"] = 4;
  symbols_["SCREEN"] = 16384;
  symbols_["KBD"] = 24576;
}

void Assembler::firstPass(const std::vector<std::string> &lines) {
  int counter = 0; // ROM address of the next A/C instruction
  for (const std::string &line : lines) {
    if (classify(line) == InstructionType::L_INSTRUCTION) {
      std::string name = line.substr(1, line.size() - 2);
      if (symbols_.find(name) == symbols_.end())
        symbols_[name] = counter; // first definition wins
    } else {
      counter++; // A or C (ERROR lines counted too; writer rejects them)
    }
  }
}

int Assembler::resolveSymbol(const std::string &name) {
  auto it = symbols_.find(name);
  if (it != symbols_.end())
    return it->second;
  int addr = nextVar_++;
  symbols_[name] = addr;
  return addr;
}
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
    {"0", "0101010"},   {"1", "0111111"},   {"-1", "0111010"},
    {"D", "0001100"},   {"A", "0110000"},   {"!D", "0001101"},
    {"!A", "0110001"},  {"-D", "0001111"},  {"-A", "0110011"},
    {"D+1", "0011111"}, {"A+1", "0110111"}, {"D-1", "0001110"},
    {"A-1", "0110010"}, {"D+A", "0000010"}, {"D-A", "0010011"},
    {"A-D", "0000111"}, {"D&A", "0000000"}, {"D|A", "0010101"},
    {"M", "1110000"},   {"!M", "1110001"},  {"-M", "1110011"},
    {"M+1", "1110111"}, {"M-1", "1110010"}, {"D+M", "1000010"},
    {"D-M", "1010011"}, {"M-D", "1000111"}, {"D&M", "1000000"},
    {"D|M", "1010101"},
};
const std::unordered_map<std::string, std::string> kDest = {
    {"", "000"},  {"M", "001"},  {"D", "010"},  {"MD", "011"},
    {"A", "100"}, {"AM", "101"}, {"AD", "110"}, {"AMD", "111"},
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

bool Assembler::isDecimal(const std::string &payload) {
  try {
    size_t pos;
    int n = std::stoi(payload, &pos, 10);

    if (pos != payload.size())
      return false;

    if (n < 0 || n > 32767)
      return false;

    return true;

  } catch (const std::invalid_argument &e) {
    return false;
  } catch (const std::out_of_range &e) {
    return false;
  }
}

namespace {
// 15-bit value -> 15-char MSB-first string ("000000000000101" for 5).
std::string ToBits15(int v) {
  std::string s(15, '0');
  for (int i = 0; i < 15; i++)
    if (v & (1 << i))
      s[14 - i] = '1';
  return s;
}
} // namespace

bool Assembler::assemble(std::vector<std::string> &lines,
                         std::vector<std::string> &out) {
  firstPass(lines);
  for (size_t i = 0; i < lines.size(); i++) {
    InstructionType type = classify(lines[i]);
    if (type == InstructionType::A_INSTRUCTION) {
      std::string sym = lines[i].substr(1);
      int addr = isDecimal(sym) ? std::stoi(sym) : resolveSymbol(sym);
      if (addr < 0 || addr > 32767)
        return false;
      out.push_back("0" + ToBits15(addr));
    } else if (type == InstructionType::C_INSTRUCTION) {
      C_split parts = C_Splitter(lines[i]);
      std::string comp_bits = compBits(parts.comp);
      std::string dest_bits = destBits(parts.dest);
      std::string jump_bits = jumpBits(parts.jump);
      if (comp_bits == "" || dest_bits == "" || jump_bits == "")
        return false;
      out.push_back("111" + comp_bits + dest_bits + jump_bits);
    } else if (type == InstructionType::ERROR) {
      return false;
    }
    // LABEL: emits nothing.
  }
  return true;
}
} // namespace bitforge::assembler
