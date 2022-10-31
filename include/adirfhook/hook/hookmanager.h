#ifndef ADIRFHOOK_HOOK_HOOKMANAGER_H_
#define ADIRFHOOK_HOOK_HOOKMANAGER_H_

#include <atomic>
#include <unordered_map>

#include "gum/gum.h"

namespace adirfhook {

class HookContext {
 public:
  HookContext(uint64_t hook_id, void* hook_target) 
    : hook_id_(hook_id), hook_target_(hook_target) {} 

  uint64_t HookId() const { return hook_id_; }
  void* HookTarget() const { return hook_target_; }
 
 private:
  uint64_t hook_id_;
  void* hook_target_;
}; // class HookContext

class HookManager {
 public:
  using HookId = uint64_t;
  using HookCallback = std::function<void(HookContext*)>;

  HookManager();
  ~HookManager();

  HookId Hook(void* function_addr, HookCallback callback);
  bool Unhook(HookId hook_id);

 private:
  GumInterceptor* interceptor_;
  GumInvocationListener* default_listener_;
  std::atomic<HookId> next_hook_id_ = ATOMIC_VAR_INIT(0);
  std::unordered_map<HookId, HookContext*> hook_contexts_;

};  // class DlopenHook

}  // namespace adirfhook

#endif  // ADIRFHOOK_HOOK_HOOKMANAGER_H_