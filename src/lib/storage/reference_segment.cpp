#include <utils/performance_warning.hpp>
#include "reference_segment.hpp"
#include "table.hpp"


namespace opossum {
    ReferenceSegment::ReferenceSegment(const std::shared_ptr<const Table> referenced_table,
                                       const ColumnID referenced_column_id, const std::shared_ptr<const PosList> pos) :
                                       _pos_list(pos), _referenced_table(referenced_table), _referenced_column_id(referenced_column_id){

    }

    AllTypeVariant ReferenceSegment::operator[](const ChunkOffset chunk_offset) const {
        PerformanceWarning("operator[] used");

        // Get row ID for lookup in reference table from position list
        const auto row = _pos_list->at(chunk_offset);

        return _referenced_table->get_chunk(row.chunk_id)
                .get_segment(_referenced_column_id)
                ->operator[](row.chunk_offset);
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

    std::shared_ptr<const PosList>
    ReferenceSegment::scan(const ScanType scan_type, const AllTypeVariant search_value, const ChunkID chunk_id) const {

        bool in_scope;
        PosList pos_list_after_scan = PosList(); //Can we reuse the old pos list  to avoid reallocating values?

        size_t size_of_pos_list = _pos_list->size();

        //Loop though all entries from pos_list and check if they are in scan range
        for (size_t pos_list_id=0; pos_list_id < size_of_pos_list; ++pos_list_id) {

            RowID row_id = _pos_list->at(pos_list_id);

            AllTypeVariant value = _referenced_table->get_chunk(row_id.chunk_id)
                .get_segment(_referenced_column_id)
                ->operator[](row_id.chunk_offset); //TODO Should we use another operator/method to get value here?

            in_scope = scan_compare(scan_type, value, search_value);
            if (in_scope) {
                pos_list_after_scan.emplace_back(row_id);
            }
        }
        return std::make_shared<PosList>(pos_list_after_scan);
    }

    size_t ReferenceSegment::estimate_memory_usage() const {
       return _pos_list->size() * sizeof(RowID) +
       sizeof(std::shared_ptr<const PosList>) +
       sizeof(std::shared_ptr<const Table>) +
       sizeof(ColumnID);
    };
}