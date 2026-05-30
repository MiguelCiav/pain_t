#include "shortcut_manager.h"

void shortcut_manager::register_shortcut(int key, bool ctrl, std::function<void()> action) {
    bindings[{key, ctrl}] = action;
}

bool shortcut_manager::handle_key(int key, bool ctrl) {
    auto it = bindings.find({key, ctrl});
    if (it != bindings.end()) {
        it->second();
        return true;
    }
    return false;
}
