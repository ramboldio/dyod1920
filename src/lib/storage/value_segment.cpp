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
#include "types.hpp"

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
  return sizeof(T) * _value_segment.size();
}

template <typename T>
const std::vector<T>& ValueSegment<T>::values() const {
  return _value_segment;
}


//TODO I don't like the chunk_id parameter. But it is needed to construct the pos list
//Clean but unefficient would be to return a vector<ChunkOffset> and construct the poslist within the chunk
template<typename T>
void ValueSegment<T>::scan(const ScanType scan_type, const AllTypeVariant untyped_search_value, const ChunkID chunk_id, std::shared_ptr<PosList> pos_list) const {
    T search_value = type_cast<T>(untyped_search_value);

    bool in_scope; // Create boolean value before doing the for loop to allocate memory only once.
    size_t size_of_value_segment = _value_segment.size();
    for (size_t chunk_offset=0; chunk_offset < size_of_value_segment; ++chunk_offset) {
        in_scope = scan_compare(scan_type, _value_segment.at(chunk_offset), search_value);
        if (in_scope){
            pos_list->emplace_back(RowID({
                chunk_id,
                ChunkOffset(chunk_offset)}
                ));
        }
    }
}

    EXPLICITLY_INSTANTIATE_DATA_TYPES(ValueSegment);

}  // namespace opossum
