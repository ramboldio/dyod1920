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
  tables[name] = table; //what happens if the table already exists?
}

void StorageManager::drop_table(const std::string& name) {
    auto it=tables.find(name);
    tables.erase (it);
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
  // Implementation goes here
}

void StorageManager::reset() {
    // Implementation goes here
}

}  // namespace opossum
