#include <resolve_type.hpp>
#include "table_scan.hpp"

#include "../storage/table.hpp"
#include "../storage/chunk.hpp"

#include "../storage/value_segment.hpp"
#include "../storage/dictionary_segment.hpp"
#include "../storage/reference_segment.hpp"

#include "../storage/table.hpp"

namespace opossum {

    //Constructor
    TableScan::TableScan(const std::shared_ptr<const AbstractOperator> in, ColumnID column_id, const ScanType scan_type,
                         const AllTypeVariant search_value):
                         _input_operator(in),
                         _column_id(column_id),
                         _scan_type(scan_type),
                         _search_value(search_value) {

    }

    //Deconstructor
    TableScan::~TableScan() {

    }

    //Protected function
    std::shared_ptr<const Table> TableScan::_on_execute() {

        //extract Table from previous operator
        std::shared_ptr<const Table> input_table = _input_operator->get_output();

        //Create new table which is a view on the old one.
        Table view_table = Table();
        view_table.add_column(input_table->column_name(_column_id), input_table->column_type(_column_id));

        ChunkID chunk_count = input_table->chunk_count();
        for (ChunkID chunk_id = ChunkID(0); chunk_id < chunk_count; ++chunk_id){
            std::shared_ptr<const PosList> pos_list = input_table->get_chunk(chunk_id)
                    .get_segment(_column_id)
                    ->scan(_scan_type, _search_value, chunk_id);

            //Create new table and new
            if (pos_list->size() > 0){
                ReferenceSegment reference_segment = ReferenceSegment(input_table,_column_id, pos_list);

                //Add all reference segments to the first Chunk
                view_table.get_chunk(ChunkID(0)).add_segment(std::make_shared<ReferenceSegment>(reference_segment));
            }
        }
        return std::make_shared<const Table> (view_table);
    }

    //Getter
    ColumnID TableScan::column_id() const {
        return _column_id;
    }

    ScanType TableScan::scan_type() const {
        return _scan_type;
    }

    const AllTypeVariant &TableScan::search_value() const {
        return _search_value;
    }
}

