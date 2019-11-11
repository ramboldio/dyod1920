#pragma once

#include "types.hpp"
#include "base_attribute_vector.hpp"

namespace opossum {

template <typename T>
class FixedSizeAttributeVector : public BaseAttributeVector {
    public:
        FixedSizeAttributeVector() {
            _vector = std::vector<T>();
        }

        ~FixedSizeAttributeVector() = default;

        // we need to explicitly set the move constructor to default when
        // we overwrite the copy constructor
        FixedSizeAttributeVector(FixedSizeAttributeVector&&) = default;
        FixedSizeAttributeVector& operator=(FixedSizeAttributeVector&&) = default;

        // returns the value id at a given position
        ValueID get(const size_t i) const override {
            return static_cast<ValueID>(_vector.at(i));
        };

        // sets the value id at a given position
        void set(const size_t i, const ValueID value_id) override {
            _vector.insert(_vector.cbegin() + i, static_cast<T>(value_id));
        };

        // returns the number of values
        size_t size() const override {
            return _vector.size();
        };

        // returns the width of biggest value id in bytes
        AttributeVectorWidth width() const override {
            return sizeof(T);
        };

    protected:
        std::vector<T> _vector;
    };
}  // namespace opossum
