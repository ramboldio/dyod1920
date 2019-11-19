#pragma once

#include <memory>
#include <string>
#include <vector>
#include <storage/storage_manager.hpp>

#include "abstract_operator.hpp"

namespace opossum {

// operator to retrieve a table from the StorageManager by specifying its name
class GetTable : public AbstractOperator {
 public:
  explicit GetTable(const std::string& name) {
      auto& sm = StorageManager::get();
      _table = sm.get_table(name);
      _name = name;
  }

    const std::string table_name() const {
        return _name;
    }

protected:
  std::shared_ptr<const Table> _on_execute() override {
      return _table;
  }
  // TODO checkout whether we could use const here
  std::string _name;
  std::shared_ptr<const Table> _table;
};
}  // namespace opossum
