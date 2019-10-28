#include "value_segment.hpp"

#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "type_cast.hpp"
#include "utils/assert.hpp"
#include "utils/performance_warning.hpp"

namespace opossum {

template <typename T>
AllTypeVariant ValueSegment<T>::operator[](const ChunkOffset chunk_offset) const {
  PerformanceWarning("operator[] used");
  return value_segment[chunk_offset];
}

template <typename T>
void ValueSegment<T>::append(const AllTypeVariant& val) {
  value_segment.push_back(type_cast<T>(val));
}

template <typename T>
size_t ValueSegment<T>::size() const {
  // Why do we need size_t here and not just uint32_t since it should have
  // the same range as chunk size which is uint32
  return value_segment.size();
}

EXPLICITLY_INSTANTIATE_DATA_TYPES(ValueSegment);

}  // namespace opossum
