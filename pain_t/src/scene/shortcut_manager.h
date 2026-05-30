#pragma once
#include <unordered_map>
#include <functional>

struct key_combo {
    int key;
    bool ctrl;
    bool operator==(const key_combo& other) const {
        return key == other.key && ctrl == other.ctrl;
    }
};

namespace std {
    template <>
    struct hash<key_combo> {
        size_t operator()(const key_combo& k) const {
            return (hash<int>()(k.key) ^ (hash<bool>()(k.ctrl) << 1));
        }
    };
}

class shortcut_manager {
private:
    std::unordered_map<key_combo, std::function<void()>> bindings;

public:
    void register_shortcut(int key, bool ctrl, std::function<void()> action);
    bool handle_key(int key, bool ctrl);
};
