#include "storage_manager.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "utils/assert.hpp"

namespace opossum {

StorageManager& StorageManager::get() {
  static StorageManager instance;
  return instance;
}

void StorageManager::add_table(const std::string& name, std::shared_ptr<Table> table) {
  if (!this->has_table(name)) {
    tables[name] = table;
  } else {
    throw std::runtime_error(std::string("Table already exists: " + name));
  }
}

void StorageManager::drop_table(const std::string& name) {
  this->get_table(name);
  tables.erase(name);
}

std::shared_ptr<Table> StorageManager::get_table(const std::string& name) const {
  auto it = tables.find(name);
  if (it != tables.end()) {
    return it->second;
  }

  throw std::runtime_error(std::string("Cannot find following table: " + name));
}

bool StorageManager::has_table(const std::string& name) const {
  auto it = tables.find(name);
  if (it != tables.end()) {
    return true;
  }
  return false;
}

std::vector<std::string> StorageManager::table_names() const {
  throw std::runtime_error("Implement StorageManager::table_names");
}

void StorageManager::print(std::ostream& out) const {
  for (auto table : tables) {
    out << table.first << std::endl;
  }
}

void StorageManager::reset() { tables.clear(); }

}  // namespace opossum
