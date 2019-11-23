#include <utils/performance_warning.hpp>
#include "reference_segment.hpp"


namespace opossum {
    ReferenceSegment::ReferenceSegment(const std::shared_ptr<const Table> referenced_table,
                                       const ColumnID referenced_column_id, const std::shared_ptr<const PosList> pos) :
                                       _pos_list(pos), _referenced_table(referenced_table), _referenced_column_id(referenced_column_id){

    }

    AllTypeVariant ReferenceSegment::operator[](const ChunkOffset chunk_offset) const {
        PerformanceWarning("operator[] used");

        // Get row ID for lookup in reference table from position list
        const auto row = _pos_list->at(chunk_offset);

        auto value = _referenced_table->get_chunk(row.chunk_id)
                .get_segment(_referenced_column_id)
                ->operator[](row.chunk_offset);

        //TODO Use posList to get a specfic value from the segment
        return value;
    }

    size_t ReferenceSegment::size() const {
        return _pos_list->size();
    }

    const std::shared_ptr<const PosList> ReferenceSegment::pos_list() const {
        return _pos_list;
    }

    const std::shared_ptr<const Table> ReferenceSegment::referenced_table() const {
        return _referenced_table;
    }

    ColumnID ReferenceSegment::referenced_column_id() const {
        return _referenced_column_id;
    }

    size_t ReferenceSegment::estimate_memory_usage() const {
       return _pos_list->size() * sizeof(RowID) +
       sizeof(std::shared_ptr<const PosList>) +
       sizeof(std::shared_ptr<const Table>) +
       sizeof(ColumnID);
    };
}