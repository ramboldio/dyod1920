#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "../base_test.hpp"
#include "gtest/gtest.h"

#include "operators/abstract_operator.hpp"
#include "operators/get_table.hpp"
#include "operators/print.hpp"
#include "operators/table_scan.hpp"
#include "storage/reference_segment.hpp"
#include "storage/storage_manager.hpp"
#include "storage/table.hpp"
#include "types.hpp"
#include "storage/chunk.hpp"

namespace opossum {

 class ReferenceSegmentTest : public BaseTest {
   virtual void SetUp() {
     _test_table = std::make_shared<opossum::Table>(opossum::Table(3));
     _test_table->add_column("a", "int");
     _test_table->add_column("b", "float");
     _test_table->append({123, 456.7f});
     _test_table->append({1234, 457.7f});
     _test_table->append({12345, 458.7f});
     _test_table->append({54321, 458.7f});
     _test_table->append({12345, 458.7f});

     _test_table_dict = std::make_shared<opossum::Table>(5);
     _test_table_dict->add_column("a", "int");
     _test_table_dict->add_column("b", "int");
     for (int i = 0; i <= 24; i += 2) _test_table_dict->append({i, 100 + i});

     _test_table_dict->compress_chunk(ChunkID(0));
     _test_table_dict->compress_chunk(ChunkID(1));

     StorageManager::get().add_table("test_table_dict", _test_table_dict);
   }

  public:
   std::shared_ptr<opossum::Table> _test_table, _test_table_dict;
 };

 TEST_F(ReferenceSegmentTest, IsImmutable) {
   auto pos_list =
       std::make_shared<PosList>(std::initializer_list<RowID>({{ChunkID{0}, 0}, {ChunkID{0}, 1}, {ChunkID{0}, 2}}));
   auto reference_segment = ReferenceSegment(_test_table, ColumnID{0}, pos_list);

   EXPECT_THROW(reference_segment.append(1), std::logic_error);
 }

 TEST_F(ReferenceSegmentTest, RetrievesValues) {
   // PosList with (0, 0), (0, 1), (0, 2)
   auto pos_list = std::make_shared<PosList>(
       std::initializer_list<RowID>({RowID{ChunkID{0}, 0}, RowID{ChunkID{0}, 1}, RowID{ChunkID{0}, 2}}));
   auto reference_segment = ReferenceSegment(_test_table, ColumnID{0}, pos_list);

   auto& column = *(_test_table->get_chunk(ChunkID{0}).get_segment(ColumnID{0}));

   EXPECT_EQ(reference_segment[0], column[0]);
   EXPECT_EQ(reference_segment[1], column[1]);
   EXPECT_EQ(reference_segment[2], column[2]);
 }

 TEST_F(ReferenceSegmentTest, RetrievesValuesOutOfOrder) {
   // PosList with (0, 1), (0, 2), (0, 0)
   auto pos_list = std::make_shared<PosList>(
       std::initializer_list<RowID>({RowID{ChunkID{0}, 1}, RowID{ChunkID{0}, 2}, RowID{ChunkID{0}, 0}}));
   auto reference_segment = ReferenceSegment(_test_table, ColumnID{0}, pos_list);

   auto& column = *(_test_table->get_chunk(ChunkID{0}).get_segment(ColumnID{0}));

   EXPECT_EQ(reference_segment[0], column[1]);
   EXPECT_EQ(reference_segment[1], column[2]);
   EXPECT_EQ(reference_segment[2], column[0]);
 }

 TEST_F(ReferenceSegmentTest, RetrievesValuesFromChunks) {
   // PosList with (0, 2), (1, 0), (1, 1)
   auto pos_list = std::make_shared<PosList>(
       std::initializer_list<RowID>({RowID{ChunkID{0}, 2}, RowID{ChunkID{1}, 0}, RowID{ChunkID{1}, 1}}));
   auto reference_segment = ReferenceSegment(_test_table, ColumnID{0}, pos_list);

   auto& column_1 = *(_test_table->get_chunk(ChunkID{0}).get_segment(ColumnID{0}));
   auto& column_2 = *(_test_table->get_chunk(ChunkID{1}).get_segment(ColumnID{0}));

   EXPECT_EQ(reference_segment[0], column_1[2]);
   EXPECT_EQ(reference_segment[2], column_2[1]);
 }

 TEST_F(ReferenceSegmentTest, ReferenceSegmentScan) {
    // values from 0 - 99

    auto table = std::make_shared<Table>(Table(20));
    table->add_column("a", "int");
    table->add_column("b", "string");

    //90 values
    for (int i = 0; i < 90; i++){
        table->append({i, "hello_test_" + std::to_string(i)});
    }
    DebugAssert(table->chunk_count() == 5, "Table chunk count is not correct");

    const auto chunk_id = ChunkID(4);
    const auto column_id = ColumnID(0);
    auto last_segment = table->get_chunk(chunk_id).get_segment(column_id);
    DebugAssert(last_segment->size() == 10, "Last chunk does not have 10 entries");


    //Do scan on last segment
    //Segment has int values from 80 - 89
    auto ref_pos_list = std::make_shared<PosList>(PosList());
    last_segment->scan(ScanType::OpGreaterThan, 83, chunk_id, ref_pos_list);

    DebugAssert(ref_pos_list->size() == 6, "Pos_list does not have 6 entries");

    //Create ReferenceSegment

    auto reference_segment = ReferenceSegment(table, column_id, ref_pos_list);

    //Scan
    auto pos_list = std::make_shared<PosList>(PosList());
    reference_segment.scan(ScanType::OpEquals, 87, chunk_id, pos_list);

    EXPECT_EQ(pos_list->size(), 1);

    auto check_value = reference_segment[pos_list->at(0).chunk_offset];

    EXPECT_EQ(type_cast<int>(check_value), 87);
 }

}  // namespace opossum
