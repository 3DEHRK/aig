#pragma once
#include <string>
#include <SFML/System.hpp>

class PlayState;

namespace SaveGame {
    bool save(const PlayState& state, const std::string& path);
    bool load(PlayState& state, const std::string& path);
}
