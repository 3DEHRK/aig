#include "Item.h"
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

struct ItemDef { std::string name; std::string desc; };
static std::unordered_map<std::string, ItemDef> g_itemDefs;

void LoadItemDefinitions(const std::string& path = "data/items_basic.json") {
    std::ifstream is(path); if (!is) { std::cerr << "Item defs missing: "<<path<<"\n"; return; }
    try { nlohmann::json j; is >> j; if (!j.is_object() || !j.contains("items")) return; for (auto &e : j["items"]) {
        if (!e.contains("id")) continue; std::string id = e["id"].get<std::string>(); std::string nm = e.value("name", id); std::string dc = e.value("desc", ""); g_itemDefs[id] = {nm, dc}; }
        std::cerr << "Loaded "<<g_itemDefs.size()<<" item defs\n";
    } catch (...) { std::cerr << "Failed parsing item defs\n"; }
}

ItemPtr MakeItem(const std::string& id, int count = 1) {
    auto it = g_itemDefs.find(id);
    if (it == g_itemDefs.end()) return std::make_shared<Item>(id, id, "", count);
    return std::make_shared<Item>(id, it->second.name, it->second.desc, count);
}