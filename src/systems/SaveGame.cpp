#include "SaveGame.h"
#include "../states/PlayState.h"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

bool SaveGame::save(const PlayState& state, const std::string& path) {
    json j;
    // minimal: delegate to PlayState to write its data via public getters (we'll add getters shortly)
    // placeholder: create empty save
    j["version"] = 1;
    std::ofstream os(path);
    if (!os) return false;
    os << j.dump(4);
    return true;
}

bool SaveGame::load(PlayState& state, const std::string& path) {
    std::ifstream is(path);
    if (!is) return false;
    json j; is >> j;
    // placeholder: nothing to load
    return true;
}
