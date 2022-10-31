#include "adirfhook/hook/dlopenhook.h"

#include <algorithm> // std::remove

using namespace adirfhook;

bool DlopenHook::Hook() {
  return false;  // TODO
}

bool DlopenHook::Unhook() {
  return false;  // TODO
}

void DlopenHook::RegisterDlopenCallback(DlopenCallback callback) {
    callbacks_.emplace_back(callback);
}

void DlopenHook::UnregisterDlopenCallback(DlopenCallback callback) {
    std::remove(callbacks_.begin(), callbacks_.end(), callback);
}
