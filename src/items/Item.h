#pragma once
#include <string>
#include <memory>

struct Item {
    std::string id;
    std::string name;
    std::string description;
    int stackSize = 1;
    std::string iconPath; // optional path to an icon texture

    Item() = default;
    Item(std::string id_, std::string name_, std::string desc_, int stack = 1, std::string icon = "")
    : id(std::move(id_)), name(std::move(name_)), description(std::move(desc_)), stackSize(stack), iconPath(std::move(icon)) {}
};
using ItemPtr = std::shared_ptr<Item>;