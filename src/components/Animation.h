#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <unordered_map>

struct Animation {
    std::string name;
    std::vector<sf::IntRect> frames;
    float frameDuration = 0.1f; // seconds per frame
    bool loop = true;

    Animation() = default;
    Animation(std::string n, float dur = 0.1f, bool lp = true)
    : name(std::move(n)), frameDuration(dur), loop(lp) {}

    void addFrame(const sf::IntRect& r) { frames.push_back(r); }
    bool empty() const { return frames.empty(); }
    std::size_t size() const { return frames.size(); }
};

// Simple Animator class declaration + inline implementation
// kept in header so other translation units (AnimatedSprite) can use it.
class Animator {
public:
    Animator() = default;

    void addAnimation(const Animation& anim) {
        animations[anim.name] = anim;
    }

    void play(const std::string& name, bool restart = false) {
        if (current == name && !restart) return;
        auto it = animations.find(name);
        if (it == animations.end()) return;
        current = name;
        elapsed = 0.f;
        frameIndex = 0;
    }

    void stop() { current.clear(); frameIndex = 0; elapsed = 0.f; }

    // advance animation; returns current frame rect (or nullptr if none)
    const sf::IntRect* update(float dt) {
        if (current.empty()) return nullptr;
        auto it = animations.find(current);
        if (it == animations.end()) return nullptr;
        const Animation& a = it->second;
        if (a.frames.empty()) return nullptr;

        elapsed += dt;
        while (elapsed >= a.frameDuration) {
            elapsed -= a.frameDuration;
            ++frameIndex;
            if (frameIndex >= a.frames.size()) {
                if (a.loop) frameIndex = 0;
                else { frameIndex = int(a.frames.size()) - 1; break; }
            }
        }
        return &a.frames[frameIndex];
    }

    const Animation* getCurrentAnimation() const {
        auto it = animations.find(current);
        return it != animations.end() ? &it->second : nullptr;
    }

private:
    std::unordered_map<std::string, Animation> animations;
    std::string current;
    float elapsed = 0.f;
    std::size_t frameIndex = 0;
};