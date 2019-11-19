#include <iomanip>
#include <iterator>
#include <limits>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "base_segment.hpp"
#include "chunk.hpp"

#include "utils/assert.hpp"

namespace opossum {

    void Chunk::add_segment(std::shared_ptr<BaseSegment> segment) {
    column_segments.push_back(segment);
}

void Chunk::append(const std::vector<AllTypeVariant>& values) {
  DebugAssert(values.size() == column_count(),
              "Number of given values do not match up with number of segments within the chunk");

  int values_size = values.size();
  for (int index = 0; index < values_size; ++index) {
    column_segments[index].get()->append(values[index]);
  }
}

std::shared_ptr<BaseSegment> Chunk::get_segment(ColumnID column_id) const { return column_segments.at((int)column_id); }

uint16_t Chunk::column_count() const { return column_segments.size(); }

uint32_t Chunk::size() const { return column_segments.size() != 0 ? column_segments.front().get()->size() : 0; }

}  // namespace opossum
