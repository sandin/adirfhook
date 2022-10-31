#include <android/log.h>
#include <fcntl.h>
#include <jni.h>
#include <unistd.h>

#include "gum/gum.h"
#include "adirfhook/hook/hookmanager.h"

using namespace adirfhook;

static void test() {
  HookManager mgr;

  gpointer open_addr = GSIZE_TO_POINTER(gum_module_find_export_by_name(NULL, "close"));
  HookManager::HookId hook_id = mgr.Hook(open_addr, [=](HookContext* ctx) {
    __android_log_print(ANDROID_LOG_INFO, "adirfhook", "hook close"); 
  });
  // TODO

  close(open("/proc/self/maps", O_RDONLY));


  mgr.Unhook(hook_id);
}

JNIEXPORT jint __attribute__((visibility("default")))
JNI_OnLoad(JavaVM* vm, void* reserved) {
  __android_log_print(ANDROID_LOG_INFO, "adirfhook", "JNI_OnLoad");
  test();
  JNIEnv* env = NULL;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return JNI_ERR;
  }

  return JNI_VERSION_1_6;
}
