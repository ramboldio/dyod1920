#include "table.hpp"

#include <algorithm>
#include <iomanip>
#include <limits>
#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include "value_segment.hpp"
#include "dictionary_segment.hpp"

#include "resolve_type.hpp"
#include "types.hpp"
#include "utils/assert.hpp"
#include "dictionary_segment.hpp"

namespace opossum {

Table::Table(uint32_t chunk_size) {
  this->chunk_size = chunk_size;
  this->build_chunk();
}

void Table::add_column(const std::string& name, const std::string& type) {
  // Add column to vectors
  DebugAssert(col_names.size() == col_types.size(), "Col_names size differs from col_types size");
  col_names.push_back(name);
  col_types.push_back(type);

  auto segment = make_shared_by_data_type<BaseSegment, ValueSegment>(type);
  _chunks.back().add_segment(segment);
}

void Table::
    // TODO(all): this is a protected function. Is there a better way to define it ?
    // My IDE wrongly notifies me that I do not use this function
    build_chunk() {
  // Create Chunk
  _chunks.push_back(Chunk());

  // Create segments for new chunk
  for (uint32_t index = 0; index < col_types.size(); ++index) {  // TODO(all): Wat is the MAX for col_types?
    auto segment = make_shared_by_data_type<BaseSegment, ValueSegment>(col_types[index]);
    _chunks.back().add_segment(segment);
  }
}

void Table::append(std::vector<AllTypeVariant> values) {
  // if last chunk is full create a new chunk and add it to back
  if (_chunks.back().size() >= chunk_size) {
    this->build_chunk();
  }
  _chunks.back().append(values);
}

uint16_t Table::column_count() const {
  return _chunks.back()
      .column_count();  // I assume here that all chunks have the same count of columns. Last chunk -> most recent data
}

uint64_t Table::row_count() const { return chunk_size * (chunk_count() - 1) + _chunks.back().size(); }

ChunkID Table::chunk_count() const { return ChunkID{static_cast<uint32_t>(_chunks.size())}; }

ColumnID Table::column_id_by_name(const std::string& column_name) const {
  ColumnID index = ColumnID(distance(col_names.begin(), find(col_names.begin(), col_names.end(), column_name)));

  // if index is equal to the vector size -> name is not in vector because iterator moved over all entities
  if (index.t < col_names.size()) {
    return index;
  }
  throw std::runtime_error(std::string("Cannot find following column_name: " + column_name));
}

uint32_t Table::max_chunk_size() const { return chunk_size; }

const std::vector<std::string>& Table::column_names() const { return col_names; }

const std::string& Table::column_name(ColumnID column_id) const { return col_names[column_id]; }

const std::string& Table::column_type(ColumnID column_id) const { return col_types[column_id]; }

Chunk& Table::get_chunk(ChunkID chunk_id) { return _chunks[chunk_id]; }

const Chunk& Table::get_chunk(ChunkID chunk_id) const { return _chunks[chunk_id]; }

void Table::compress_chunk(ChunkID chunk_id) {

    Chunk dict_chunk = Chunk();
    Chunk& old_chunk = get_chunk(chunk_id);

    for(ColumnID i = static_cast<ColumnID>(0); i <= old_chunk.column_count(); ++i){
        const auto old_segment = old_chunk.get_segment(i);

        // Create dict segment and add it to the new dict_chunk
        auto pSegment = make_shared_by_data_type<BaseSegment, DictionarySegment>(column_type(i), old_segment);
        dict_chunk.add_segment(pSegment);
    }

    // Replace Chunk
    _chunks[chunk_id] = std::move(dict_chunk);

}

}  // namespace opossum
