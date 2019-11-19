#pragma once

#include <limits>
#include <memory>
#include <set>
#include <string>
#include <type_cast.hpp>
#include <utility>
#include <vector>

#include "all_type_variant.hpp"
#include "base_attribute_vector.hpp"
#include "fixed_size_attribute_vector.hpp"
#include "value_segment.hpp"
#include "types.hpp"

namespace opossum {

class BaseAttributeVector;
class BaseSegment;

// Even though ValueIDs do not have to use the full width of ValueID (uint32_t), this will also work for smaller ValueID
// types (uint8_t, uint16_t) since after a down-cast INVALID_VALUE_ID will look like their numeric_limit::max()
constexpr ValueID INVALID_VALUE_ID{std::numeric_limits<ValueID::base_type>::max()};

// Dictionary is a specific segment type that stores all its values in a vector
template <typename T>
class DictionarySegment : public BaseSegment {
 public:
  /**
   * Creates a Dictionary segment from a given value segment.
   */
  explicit DictionarySegment(const std::shared_ptr<BaseSegment>& base_segment) {
    _attribute_vector = std::make_shared<FixedSizeAttributeVector<uint16_t>>(FixedSizeAttributeVector<uint16_t>());

    std::set<T> set_dict = std::set<T>();

    const auto value_segment = std::dynamic_pointer_cast<ValueSegment<T>>(base_segment);

    const auto values = value_segment->values();
    //Insert values in set to delete duplicates
    for (auto value : values) {
      set_dict.emplace(value);
    }

    // Convert set to vector to create the dictionary
    const auto dict = std::vector<T>(set_dict.begin(), set_dict.end());

    // Create pointer for new dict
    _dictionary = std::make_shared<std::vector<T>>(dict);

    const auto entropy = ((int)std::log2(_dictionary->size())) + 1;
    if (entropy <= 8) {
      _attribute_vector = std::make_shared<FixedSizeAttributeVector<uint8_t>>(FixedSizeAttributeVector<uint8_t>());
    } else if (entropy <= 16) {
      _attribute_vector = std::make_shared<FixedSizeAttributeVector<uint16_t>>(FixedSizeAttributeVector<uint16_t>());
    } else if (entropy <= 32) {
      _attribute_vector = std::make_shared<FixedSizeAttributeVector<uint32_t>>(FixedSizeAttributeVector<uint32_t>());
    } else {
      throw std::runtime_error(std::string("Not enough memory"));
    }

    for (size_t i = 0; i < values.size(); i++) {
      //Get index of value from dict
      const auto it = std::find(_dictionary->begin(), _dictionary->end(), values[i]);
      const ValueID index = static_cast<ValueID>(std::distance(_dictionary->begin(), it));
      _attribute_vector->set(i, index);
    }
  }

  // SEMINAR INFORMATION: Since most of these methods depend on the template parameter, you will have to implement
  // the DictionarySegment in this file. Replace the method signatures with actual implementations.

  // return the value at a certain position. If you want to write efficient operators, back off!
  AllTypeVariant operator[](const ChunkOffset chunk_offset) const override {
    return _dictionary->at(_attribute_vector->get(chunk_offset));
  }

  // return the value at a certain position.
  T get(const size_t chunk_offset) const { return _dictionary->at(_attribute_vector->get(chunk_offset)); }

  // dictionary segments are immutable
  void append(const AllTypeVariant&) override {
    throw std::runtime_error(std::string("Dictionary segments are immutable"));
  }

  // returns an underlying dictionary
  std::shared_ptr<const std::vector<T>> dictionary() const { return _dictionary; }

  // returns an underlying data structure
  std::shared_ptr<const BaseAttributeVector> attribute_vector() const { return _attribute_vector; }

  // return the value represented by a given ValueID
  const T& value_by_value_id(ValueID value_id) const { return _dictionary->at(value_id); }

  // returns the first value ID that refers to a value >= the search value
  // returns INVALID_VALUE_ID if all values are smaller than the search value
  ValueID lower_bound(T value) const {
    // TODO: test if this really includes the case where the value is equal to the search value
    // TODO: test if this works for strings (if not, implement!)

    auto lower_bound_ref = std::lower_bound(_dictionary->cbegin(), _dictionary->cend(), value);
    if (lower_bound_ref == _dictionary->cend()) {
      return INVALID_VALUE_ID;
    }
    auto x = lower_bound_ref - _dictionary->cbegin();
    return static_cast<ValueID>(x);
  }

  //  // same as lower_bound(T), but accepts an AllTypeVariant
  ValueID lower_bound(const AllTypeVariant& value) const { return lower_bound(type_cast<T>(value)); }

  // returns the first value ID that refers to a value < the search value
  // returns INVALID_VALUE_ID if all values are smaller than or equal to the search value
  ValueID upper_bound(T value) const {
    auto upper_bound_ref = std::upper_bound(_dictionary->cbegin(), _dictionary->cend(), value);
    if (upper_bound_ref == _dictionary->cend()) {
      return INVALID_VALUE_ID;
    }
    auto x = upper_bound_ref - _dictionary->cbegin();
    return static_cast<ValueID>(x);
  }

  // same as upper_bound(T), but accepts an AllTypeVariant
  ValueID upper_bound(const AllTypeVariant& value) const { return upper_bound(type_cast<T>(value)); }

  // return the number of unique_values (dictionary entries)
  size_t unique_values_count() const { return static_cast<size_t>(_dictionary->size()); }

  // return the number of entries
  size_t size() const override { return _attribute_vector->size(); }

  // returns the calculated memory usage
  size_t estimate_memory_usage() const final {
    const auto memory_usage =
        (_attribute_vector->size()) * _attribute_vector->width() + _dictionary->size() * sizeof(T);
    return memory_usage;
  }

 protected:
  std::shared_ptr<std::vector<T>> _dictionary;
  std::shared_ptr<BaseAttributeVector> _attribute_vector;
};

}  // namespace opossum
