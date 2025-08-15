#include "Inventory.h"

Inventory::Inventory(size_t capacity)
: cap(capacity)
{
    slots.reserve(cap);
}

bool Inventory::addItem(const ItemPtr& item) {
    if (!item) return false;
    // try stacking: naive approach, stack by id if same id and stackSize > current
    for (auto& s : slots) {
        if (s && s->id == item->id) {
            s->stackSize += item->stackSize;
            return true;
        }
    }
    if (slots.size() < cap) {
        slots.push_back(item);
        return true;
    }
    return false;
}

bool Inventory::removeItemById(const std::string& id, int count) {
    for (auto it = slots.begin(); it != slots.end(); ++it) {
        if (*it && (*it)->id == id) {
            if ((*it)->stackSize > count) {
                (*it)->stackSize -= count;
                return true;
            } else {
                slots.erase(it);
                return true;
            }
        }
    }
    return false;
}

const std::vector<ItemPtr>& Inventory::items() const { return slots; }
size_t Inventory::capacity() const { return cap; }