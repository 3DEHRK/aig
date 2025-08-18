#include "SoundManager.h"
#include <stdexcept>
#include <algorithm>

SoundManager::SoundManager() {
    std::random_device rd; rng.seed(rd());
}

sf::SoundBuffer& SoundManager::buffer(const std::string& path) {
    auto it = buffers.find(path);
    if (it != buffers.end()) return *it->second;
    auto buf = std::make_unique<sf::SoundBuffer>();
    if (!buf->loadFromFile(path)) throw std::runtime_error("Failed to load sound buffer: " + path);
    auto &ref = *buf;
    buffers[path] = std::move(buf);
    return ref;
}

void SoundManager::play(const std::string& path, float volume, float pitch) {
    try {
        auto &buf = buffer(path);
        auto snd = std::make_unique<sf::Sound>(buf);
        snd->setVolume(volume);
        snd->setPitch(pitch);
        snd->play();
        active.push_back(std::move(snd));
    } catch (...) {
        // silent failure
    }
}

void SoundManager::playRandomPitch(const std::string& path, float volume, float pitchMin, float pitchMax) {
    std::uniform_real_distribution<float> dist(pitchMin, pitchMax);
    play(path, volume, dist(rng));
}

void SoundManager::update() {
    active.erase(std::remove_if(active.begin(), active.end(), [](const std::unique_ptr<sf::Sound>& s){
        return s->getStatus() == sf::Sound::Status::Stopped;
    }), active.end());
}
