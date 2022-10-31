#ifndef ADIRFHOOK_HOOK_DLOPENHOOK_H_
#define ADIRFHOOK_HOOK_DLOPENHOOK_H_

#include <functional>
#include <vector>

namespace adirfhook {

class DlopenHook {
 public:
 using DlopenCallback = std::function<void(void*, const char*)>;

  DlopenHook() {}
  ~DlopenHook() {}

  bool Hook();
  bool Unhook();

  void RegisterDlopenCallback(DlopenCallback callback);
  void UnregisterDlopenCallback(DlopenCallback callback);

 private:
  std::vector<DlopenCallback> callbacks_;

};  // class DlopenHook

}  // namespace adirfhook

#endif  // ADIRFHOOK_HOOK_DLOPENHOOK_H_