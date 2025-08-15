#include "Inventory.h"

Inventory::Inventory(size_t capacity)
: cap(capacity)
{
    slots.resize(cap, nullptr);
}

bool Inventory::addItem(const ItemPtr& item) {
    if (!item) return false;
    // try stacking: naive approach, stack by id if same id
    for (auto& s : slots) {
        if (s && s->id == item->id) {
            s->stackSize += item->stackSize;
            return true;
        }
    }
    // find first empty slot
    for (auto& s : slots) {
        if (!s) {
            s = item;
            return true;
        }
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
                *it = nullptr;
                return true;
            }
        }
    }
    return false;
}

const std::vector<ItemPtr>& Inventory::items() const { return slots; }
size_t Inventory::capacity() const { return cap; }

ItemPtr Inventory::getItem(size_t index) const {
    if (index >= slots.size()) return {};
    return slots[index];
}

void Inventory::setItem(size_t index, const ItemPtr& item) {
    if (index >= slots.size()) return;
    slots[index] = item;
}