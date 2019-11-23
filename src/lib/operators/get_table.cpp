#include <storage/table.hpp>

#include <memory>
#include <storage/storage_manager.hpp>
#include "string"

#include "get_table.hpp"

namespace opossum {

    GetTable::GetTable(const std::string& name) : _table_name(name) {
    }

    const std::string& GetTable::table_name() const {
        return _table_name;
    }

    std::shared_ptr<const Table> GetTable::_on_execute() {
        auto& sm = StorageManager::get();
        return sm.get_table(_table_name);
    }


}