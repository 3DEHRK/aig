#pragma once
#include <string>
#include <memory>

struct Item {
    std::string id;
    std::string name;
    std::string description;
    int stackSize = 1;

    Item() = default;
    Item(std::string id_, std::string name_, std::string desc_, int stack = 1)
    : id(std::move(id_)), name(std::move(name_)), description(std::move(desc_)), stackSize(stack) {}
};
using ItemPtr = std::shared_ptr<Item>;