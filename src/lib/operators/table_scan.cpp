#include "table_scan.hpp"
#include <resolve_type.hpp>

#include "../storage/chunk.hpp"
#include "../storage/table.hpp"

#include "../storage/dictionary_segment.hpp"
#include "../storage/reference_segment.hpp"
#include "../storage/value_segment.hpp"

#include "../storage/table.hpp"

namespace opossum {

//Constructor
TableScan::TableScan(const std::shared_ptr<const AbstractOperator> in, ColumnID column_id, const ScanType scan_type,
                     const AllTypeVariant search_value)
    : AbstractOperator(in, nullptr), _column_id(column_id), _scan_type(scan_type), _search_value(search_value) {}

//Deconstructor
TableScan::~TableScan() {}

//Protected function
std::shared_ptr<const Table> TableScan::_on_execute() {
  //extract Table from previous operator
  std::shared_ptr<const Table> input_table = _input_table_left();

  // TODO move to Table class
  Table view_table = Table();
  ColumnID column_count = ColumnID(input_table->column_count());

  for (ColumnID i = ColumnID(0); i < column_count; i++) {
    view_table.add_column(input_table->column_name(i), input_table->column_type(i));
  }

  // Do scan
  std::shared_ptr<PosList> pos_list = std::make_shared<PosList>();
  ChunkID chunk_count = input_table->chunk_count();

  for (ChunkID chunk_id = ChunkID(0); chunk_id < chunk_count; ++chunk_id) {
    input_table->get_chunk(chunk_id).get_segment(_column_id)->scan(_scan_type, _search_value, chunk_id, pos_list);
  }

  // create chunk in output table that only holds ReferenceSegments
  // only if there are remaining values
  if (!pos_list->empty()) {
    Chunk c = Chunk();
    for (ColumnID i = ColumnID(0); i < column_count; i++) {
      c.add_segment(std::make_shared<ReferenceSegment>(input_table, i, pos_list));
    }
    view_table.emplace_chunk(std::move(c));
  }

  return std::make_shared<const Table>(std::move(view_table));
}

//Getter
ColumnID TableScan::column_id() const { return _column_id; }

ScanType TableScan::scan_type() const { return _scan_type; }

const AllTypeVariant& TableScan::search_value() const { return _search_value; }
}  // namespace opossum
