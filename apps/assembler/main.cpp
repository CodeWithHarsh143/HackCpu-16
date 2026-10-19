#include "bitforge/asm/assembler.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: assembler <file.asm>" << std::endl;
    return 1;
  }

  std::ifstream input(argv[1]);
  if (!input.is_open()) {
    std::cerr << "Error: Cannot open file " << argv[1] << std::endl;
    return 1;
  }

  std::vector<std::string> lines;
  std::string line;
  while (std::getline(input, line)) {
    lines.push_back(line);
  }

  bitforge::assembler::Assembler a;
  std::vector<std::string> out;
  bool success = a.assemble(lines, out);
  if (!success) {
    std::cerr << "Assembly error" << std::endl;
    return 1;
  }

  std::string outPath = (argc > 2) ? argv[2] : "program.hack";
  std::ofstream output(outPath);
  if (!output.is_open()) {
    std::cerr << "Error: Cannot open output file " << outPath << std::endl;
    return 1;
  }

  for (const std::string &hex : out) {
    output << hex << std::endl;
  }
  output.close();

  return 0;
}
