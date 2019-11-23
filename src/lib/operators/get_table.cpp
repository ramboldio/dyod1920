#include <storage/table.hpp>

#include <memory>
#include <storage/storage_manager.hpp>
#include "string"

#include "get_table.hpp"

namespace opossum {

    GetTable::GetTable(const std::string& name) : _table_name(name) {
        auto& sm = StorageManager::get();
        _table = sm.get_table(name);
    }

    const std::string& GetTable::table_name() const {
        return _table_name;
    }

    std::shared_ptr<const Table> GetTable::_on_execute() {
        return _table;
    }


}