#include "bitforge/asm/assembler.hpp"
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
} // namespace bitforge::assembler
