#pragma once

#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "all_type_variant.hpp"
#include "types.hpp"

namespace opossum {

class BaseAttributeVector;
class BaseSegment;

// Even though ValueIDs do not have to use the full width of ValueID (uint32_t), this will also work for smaller ValueID
// types (uint8_t, uint16_t) since after a down-cast INVALID_VALUE_ID will look like their numeric_limit::max()
constexpr ValueID INVALID_VALUE_ID{std::numeric_limits<ValueID::base_type>::max()};
// TODO: implement this?

// Dictionary is a specific segment type that stores all its values in a vector
template <typename T>
class DictionarySegment : public BaseSegment {
 public:
  /**
   * Creates a Dictionary segment from a given value segment.
   */
  explicit DictionarySegment(const std::shared_ptr<BaseSegment>& base_segment){

  }

  // SEMINAR INFORMATION: Since most of these methods depend on the template parameter, you will have to implement
  // the DictionarySegment in this file. Replace the method signatures with actual implementations.

  // return the value at a certain position. If you want to write efficient operators, back off!
  AllTypeVariant operator[](const ChunkOffset chunk_offset) const override{
      return _dictionary.at(_attribute_vector.at(chunk_offset));
  }

  // return the value at a certain position.
  T get(const size_t chunk_offset) const{
      // TODO: check if this is really doing the same thing as the function above
      return _dictionary.at(_attribute_vector.at(chunk_offset));
  }

  // dictionary segments are immutable
  void append(const AllTypeVariant&) override{
      throw std::runtime_error('Dictionary segments are immutable!');
  }

  // returns an underlying dictionary
  std::shared_ptr<const std::vector<T>> dictionary() const{
      return _dictionary;
  }

  // returns an underlying data structure
  std::shared_ptr<const BaseAttributeVector> attribute_vector() const{
      return _attribute_vector;
  }

  // return the value represented by a given ValueID
  const T& value_by_value_id(ValueID value_id) const{
      return _dictionary.at(value_id);
  }

  // returns the first value ID that refers to a value >= the search value
  // returns INVALID_VALUE_ID if all values are smaller than the search value
  ValueID lower_bound(T value) const{
    // TODO: test if this really includes the case where the value is equal to the search value
    // TODO: test if this works for strings (if not, implement!)

    auto value = std::lower_bound(_dictionary->begin(), _dictionary->end(), value);
    if(value == _dictionary->end()){
        return INVALID_VALUE_ID;
    }
    return (value - _dictionary->begin());
  }

  // same as lower_bound(T), but accepts an AllTypeVariant
  ValueID lower_bound(const AllTypeVariant& value) const{
      return lower_bound(type_cast<T>(value));
  }

  // returns the first value ID that refers to a value < the search value
  // returns INVALID_VALUE_ID if all values are smaller than or equal to the search value
  ValueID upper_bound(T value) const{
      // TODO: test if this really excludes the case where the value is equal to the search value
      // TODO: test if this works for strings (if not, implement!)

      auto value = std::upper_bound(_dictionary->begin(), _dictionary->end(), value);
      if(value == _dictionary->end()){
          return INVALID_VALUE_ID;
      }
      return (value - _dictionary->begin());
  }

  // same as upper_bound(T), but accepts an AllTypeVariant
  ValueID upper_bound(const AllTypeVariant& value) const{
      return upper_bound(type_cast<T>(value));
  }

  // return the number of unique_values (dictionary entries)
  size_t unique_values_count() const{
      return _dictionary->size();
  }

  // return the number of entries
  size_t size() const override{
      return _attribute_vector->size();
  }

  // returns the calculated memory usage
  size_t estimate_memory_usage() const final{
      // TODO implement
    auto attr_mem = (_attribute_vector->size()) * _attribute_vector->width();
  }

 protected:
  std::shared_ptr<std::vector<T>> _dictionary;
  std::shared_ptr<BaseAttributeVector> _attribute_vector;
};

}  // namespace opossum
