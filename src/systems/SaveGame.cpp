#include "SaveGame.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include "../input/InputManager.h"

extern nlohmann::json* g_getTunablesJson();

void SaveCustomBindings(const InputManager& input, const std::string& path) {
    nlohmann::json j; nlohmann::json amap = nlohmann::json::object();
    for (auto &kv : input.bindings()) {
        amap[kv.first] = (int)kv.second;
    }
    j["keys"] = amap;
    std::ofstream os(path); if(os) os << j.dump(2);
}

bool LoadCustomBindings(InputManager& input, const std::string& path) {
    std::ifstream is(path); if(!is) return false; nlohmann::json j; try { is >> j; } catch(...) { return false; }
    if (!j.is_object() || !j.contains("keys") || !j["keys"].is_object()) return false;
    std::unordered_map<std::string, sf::Keyboard::Key> m;
    for (auto &kv : j["keys"].items()) {
        int code = kv.value().get<int>();
        if (code >= 0 && code < (int)sf::Keyboard::KeyCount) {
            m[kv.key()] = (sf::Keyboard::Key)code;
        }
    }
    input.setBindings(m);
    return true;
}