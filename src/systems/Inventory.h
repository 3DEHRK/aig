#pragma once
#include "../items/Item.h"
#include <vector>
#include <string>

class Inventory {
public:
    Inventory(size_t capacity = 32);
    bool addItem(const ItemPtr& item);        // returns true if added (stacked or new slot)
    bool removeItemById(const std::string& id, int count = 1);
    const std::vector<ItemPtr>& items() const;
    size_t capacity() const;
private:
    size_t cap;
    std::vector<ItemPtr> slots;
};