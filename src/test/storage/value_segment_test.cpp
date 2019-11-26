#include <limits>
#include <string>
#include <vector>

#include "../base_test.hpp"
#include "gtest/gtest.h"

#include "../lib/storage/value_segment.hpp"

namespace opossum {

class StorageValueSegmentTest : public BaseTest {
 protected:
  ValueSegment<int> int_value_segment;
  ValueSegment<std::string> string_value_segment;
  ValueSegment<double> double_value_segment;

  ValueSegment<int> scan_value_segment;
};

TEST_F(StorageValueSegmentTest, GetSize) {
  EXPECT_EQ(int_value_segment.size(), 0u);
  EXPECT_EQ(string_value_segment.size(), 0u);
  EXPECT_EQ(double_value_segment.size(), 0u);
}

TEST_F(StorageValueSegmentTest, AddValueOfSameType) {
  int_value_segment.append(3);
  EXPECT_EQ(int_value_segment.size(), 1u);

  string_value_segment.append("Hello");
  EXPECT_EQ(string_value_segment.size(), 1u);

  double_value_segment.append(3.14);
  EXPECT_EQ(double_value_segment.size(), 1u);
}

TEST_F(StorageValueSegmentTest, AddValueOfDifferentType) {
  int_value_segment.append(3.14);
  EXPECT_EQ(int_value_segment.size(), 1u);
  EXPECT_THROW(int_value_segment.append("Hi"), std::exception);

  string_value_segment.append(3);
  string_value_segment.append(4.44);
  EXPECT_EQ(string_value_segment.size(), 2u);

  double_value_segment.append(4);
  EXPECT_EQ(double_value_segment.size(), 1u);
  EXPECT_THROW(double_value_segment.append("Hi"), std::exception);
}

TEST_F(StorageValueSegmentTest, MemoryUsage) {
  int_value_segment.append(1);
  EXPECT_EQ(int_value_segment.estimate_memory_usage(), size_t{4});
  int_value_segment.append(2);
  EXPECT_EQ(int_value_segment.estimate_memory_usage(), size_t{8});
}

TEST_F(StorageValueSegmentTest, ScanValueSegmentOrdered) {

    // values from 0 - 99
    for (int i = 0; i <= 99; i++){
        scan_value_segment.append(i);
    }

    auto pos_list = std::make_shared<PosList>(PosList());

    scan_value_segment.scan(ScanType::OpEquals, 95, ChunkID(0), pos_list);
    EXPECT_EQ(pos_list->size(), 1);
    pos_list->clear();

    scan_value_segment.scan(ScanType::OpGreaterThan, 95, ChunkID(0), pos_list);
    EXPECT_EQ(pos_list->size(), 4);
    pos_list->clear();

    scan_value_segment.scan(ScanType::OpGreaterThanEquals, 90, ChunkID(0), pos_list);
    EXPECT_EQ(pos_list->size(), 10);
    pos_list->clear();

    scan_value_segment.scan(ScanType::OpLessThan, 30, ChunkID(0), pos_list);
    EXPECT_EQ(pos_list->size(), 30);
    pos_list->clear();

    scan_value_segment.scan(ScanType::OpLessThanEquals, 10, ChunkID(0), pos_list);
    EXPECT_EQ(pos_list->size(), 11);
    pos_list->clear();

    scan_value_segment.scan(ScanType::OpNotEquals, 80, ChunkID(0), pos_list);
    EXPECT_EQ(pos_list->size(), 99);
    pos_list->clear();
}

}  // namespace opossum
