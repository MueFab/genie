/*#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <vector>
#include "genie/read/spring/util.h"

TEST(ReverseComplement, ValidDNAString) {
  std::string input = "ACGTACGT";
  std::string expected_output = "ACGTACGT"; // Reverse complement of itself
  std::string output;

 // genie::read::spring::ReverseComplement(input, output, input.length());
  EXPECT_EQ(output, expected_output);
}

TEST(ReverseComplement, SingleBase) {
  std::string input = "A";
  std::string expected_output = "T";
  std::string output;

  genie::read::spring::ReverseComplement(input, output, input.length());
  EXPECT_EQ(output, expected_output);
}

TEST(ReverseComplement, EmptyString) {
  std::string input = "";
  std::string output;

  genie::read::spring::ReverseComplement(input, output, input.length());
  EXPECT_TRUE(output.empty());
}*/

