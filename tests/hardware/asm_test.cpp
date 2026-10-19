#include "bitforge/hardware/rom.hpp"
#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <sstream>

using namespace bitforge::hardware;

TEST_CASE("ROM32K loads .hack file", "[rom]") {
  ROM32K rom;
  std::ifstream in("tests/hardware/rom_test.hack");
  REQUIRE(r.loadStream(in));
  REQUIRE(r.readRaw(0) == 5);
  REQUIRE(r.readRaw(1) == 0);
}

TEST_CASE("Assembler A-instruction", "[assembler]") {
  Assembler a;
  std::vector<std::string> lines = {"@5"};
  std::vector<std::string> out;
  REQUIRE(a.assemble(lines, out));
  CHECK(out[0] == "0000000000000101");
}

TEST_CASE("Assembler C-instruction", "[assembler]") {
  Assembler a;
  std::vector<std::string> lines = {"D=A"};
  std::vector<std::string> out;
  REQUIRE(a.assemble(lines, out));
  CHECK(out[0] == "1110110000010000");
}

TEST_CASE("Assembler label detection", "[assembler]") {
  Assembler a;
  InstructionType t = a.classify("(LOOP)");
  REQUIRE(t == InstructionType::L_INSTRUCTION);
}
