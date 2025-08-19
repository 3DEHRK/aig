#pragma once
#include "../items/Item.h"
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

class Inventory {
public:
    Inventory(size_t capacity = 32);
    bool addItem(const ItemPtr& item);        // returns true if added (stacked or new slot)
    bool removeItemById(const std::string& id, int count = 1);
    const std::vector<ItemPtr>& items() const;
    std::vector<ItemPtr>& itemsMutable() { return slots; } // added mutable accessor
    size_t capacity() const;
    void clear();
    bool addItemById(const std::string& id, int count = 1); // simple factory by id (placeholder metadata)
    nlohmann::json toJson() const;
    void fromJson(const nlohmann::json& j);
private:
    size_t cap;
    std::vector<ItemPtr> slots;
};