#include <algorithm>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "genie/util/merge_sort/builder.h"
#include "gtest/gtest.h"

// Mock functions for reading and writing records
std::optional<int> MockReadRecord(std::ifstream& file) {
  if (int value; file >> value) {
    return value;
  }
  return std::nullopt;
}

void MockWriteRecord(std::ofstream& file, const int& value) {
  file << value << "\n";
}

bool MockSortFunction(const int& a, const int& b) { return a < b; }

// Test fixture for the Builder class
class MergeSortBuilderTest : public testing::Test {
 protected:
  using BuilderType =
      genie::util::merge_sort::Builder<int, decltype(&MockReadRecord),
                                       decltype(&MockWriteRecord),
                                       decltype(&MockSortFunction)>;
  std::string temp_file_base_name_ = "test_chunk";
};

// Test adding records to the builder
TEST_F(MergeSortBuilderTest, AddRecordTest) {
  BuilderType builder(3, temp_file_base_name_, MockReadRecord, MockWriteRecord,
                      MockSortFunction);

  builder.AddRecord(5);
  builder.AddRecord(3);
  builder.AddRecord(8);
  builder.AddRecord(1);
  builder.AddRecord(10);

  auto merger = builder.Finish(false);

  std::vector<int> result;
  while (!merger.IsEmpty()) {
    result.push_back(merger.Get());
  }

  EXPECT_EQ(result, (std::vector{1, 3, 5, 8, 10}));
}

// Test finishing without forcing file output
TEST_F(MergeSortBuilderTest, FinishWithoutForceFileTest) {
  BuilderType builder(2, temp_file_base_name_, MockReadRecord, MockWriteRecord,
                      MockSortFunction);

  builder.AddRecord(4);
  builder.AddRecord(2);
  builder.AddRecord(6);

  auto merger = builder.Finish(false);

  std::vector<int> result;
  while (!merger.IsEmpty()) {
    result.push_back(merger.Get());
  }

  EXPECT_EQ(result, (std::vector{2, 4, 6}));
}

// Test finishing with forced file output
TEST_F(MergeSortBuilderTest, FinishWithForceFileTest) {
  BuilderType builder(2, temp_file_base_name_, MockReadRecord, MockWriteRecord,
                      MockSortFunction);

  builder.AddRecord(9);
  builder.AddRecord(7);
  builder.AddRecord(10);

  auto merger = builder.Finish(true);

  std::vector<int> result;
  while (!merger.IsEmpty()) {
    result.push_back(merger.Get());
  }

  EXPECT_EQ(result, (std::vector{7, 9, 10}));
}

// Test empty builder
TEST_F(MergeSortBuilderTest, EmptyBuilderTest) {
  BuilderType builder(2, temp_file_base_name_, MockReadRecord, MockWriteRecord,
                      MockSortFunction);

  const auto merger = builder.Finish(false);

  EXPECT_TRUE(merger.IsEmpty());
}
