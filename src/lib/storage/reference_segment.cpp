#include "reference_segment.hpp"
#include <resolve_type.hpp>
#include <utils/performance_warning.hpp>
#include "table.hpp"

namespace opossum {
ReferenceSegment::ReferenceSegment(const std::shared_ptr<const Table> referenced_table,
                                   const ColumnID referenced_column_id, const std::shared_ptr<const PosList> pos)
    : _pos_list(pos), _referenced_table(referenced_table), _referenced_column_id(referenced_column_id) {}

AllTypeVariant ReferenceSegment::operator[](const ChunkOffset chunk_offset) const {
  PerformanceWarning("operator[] used");

  // Get row ID for lookup in reference table from position list
  return _referenced_table->get_value(_pos_list->at(chunk_offset), _referenced_column_id);
}

size_t ReferenceSegment::size() const { return _pos_list->size(); }

const std::shared_ptr<const PosList> ReferenceSegment::pos_list() const { return _pos_list; }

const std::shared_ptr<const Table> ReferenceSegment::referenced_table() const { return _referenced_table; }

ColumnID ReferenceSegment::referenced_column_id() const { return _referenced_column_id; }

void ReferenceSegment::scan(const ScanType scan_type, const AllTypeVariant search_value, const ChunkID chunk_id,
                            std::shared_ptr<PosList> pos_list) const {
  bool in_scope;

  size_t size_of_pos_list = _pos_list->size();
  const std::string& data_type = _referenced_table->column_type(_referenced_column_id);

  //Loop though all entries from pos_list and check if they are in scan range
  for (size_t pos_list_id = 0; pos_list_id < size_of_pos_list; ++pos_list_id) {
    RowID row_id = _pos_list->at(pos_list_id);

    AllTypeVariant value = _referenced_table->get_value(
        row_id, _referenced_column_id);  //TODO Should we use another operator/method to get value here?

    resolve_data_type(data_type, [&](auto type) {
      using Type = typename decltype(type)::type;
      in_scope = scan_compare(scan_type, type_cast<Type>(value), type_cast<Type>(search_value));
      if (in_scope) {
        pos_list->emplace_back(RowID{ChunkID(0), ChunkOffset(pos_list_id)});
      }
    });
  }
}

size_t ReferenceSegment::estimate_memory_usage() const {
  return _pos_list->size() * sizeof(RowID) + sizeof(std::shared_ptr<const PosList>) +
         sizeof(std::shared_ptr<const Table>) + sizeof(ColumnID);
};
}  // namespace opossum