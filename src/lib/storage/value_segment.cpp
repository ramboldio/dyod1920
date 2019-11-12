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
  return _value_segment.at(chunk_offset);
}

template <typename T>
void ValueSegment<T>::append(const AllTypeVariant& val) {
  _value_segment.push_back(type_cast<T>(val));
}

template <typename T>
size_t ValueSegment<T>::size() const {
  // Why do we need size_t here and not just uint32_t since it should have
  // the same range as chunk size which is uint32
  return _value_segment.size();
}

template <typename T>
size_t ValueSegment<T>::estimate_memory_usage() const {
  // Implementation goes here
  // TODO implement
  return 0;
}

    template<typename T>
    const std::vector<T> &ValueSegment<T>::values() const {
        return _value_segment;
    }

    EXPLICITLY_INSTANTIATE_DATA_TYPES(ValueSegment);

}  // namespace opossum
