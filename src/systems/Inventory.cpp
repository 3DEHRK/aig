#include "Inventory.h"

Inventory::Inventory(size_t capacity)
: cap(capacity)
{
    slots.reserve(cap);
}

bool Inventory::addItem(const ItemPtr& item) {
    if (!item) return false;
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

void Inventory::clear() { slots.clear(); }

bool Inventory::addItemById(const std::string& id, int count) {
    if (count <= 0) return false;
    // simple placeholder naming; real system would look up item data
    auto item = std::make_shared<Item>(id, id, "", count);
    return addItem(item);
}

nlohmann::json Inventory::toJson() const {
    nlohmann::json arr = nlohmann::json::array();
    for (auto &it : slots) {
        if (it) {
            nlohmann::json entry; entry["id"] = it->id; entry["count"] = it->stackSize; arr.push_back(entry);
        }
    }
    return arr;
}

void Inventory::fromJson(const nlohmann::json& j) {
    clear();
    if (!j.is_array()) return;
    for (auto &e : j) {
        if (e.contains("id")) {
            std::string id = e["id"].get<std::string>();
            int count = e.contains("count")? e["count"].get<int>() : 1;
            addItemById(id, count);
        }
    }
}

const std::vector<ItemPtr>& Inventory::items() const { return slots; }
size_t Inventory::capacity() const { return cap; }