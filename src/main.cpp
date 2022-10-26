#include <android/log.h>
#include <fcntl.h>
#include <jni.h>
#include <unistd.h>

#include "gum/gum.h"

typedef struct _ExampleListener ExampleListener;

struct _ExampleListener {
  GObject parent;

  guint num_calls;
};

enum ExampleHookId { EXAMPLE_HOOK_OPEN, EXAMPLE_HOOK_CLOSE };

static void example_listener_iface_init(gpointer g_iface, gpointer iface_data);

#define EXAMPLE_TYPE_LISTENER (example_listener_get_type())
G_DECLARE_FINAL_TYPE(ExampleListener, example_listener, EXAMPLE, LISTENER,
                     GObject)
G_DEFINE_TYPE_EXTENDED(ExampleListener, example_listener, G_TYPE_OBJECT, 0,
                       G_IMPLEMENT_INTERFACE(GUM_TYPE_INVOCATION_LISTENER,
                                             example_listener_iface_init))

int hook_test() {
  GumInterceptor* interceptor;
  GumInvocationListener* listener;

  gum_init_embedded();

  interceptor = gum_interceptor_obtain();
  listener = (GumInvocationListener*)g_object_new(EXAMPLE_TYPE_LISTENER, NULL);

  gum_interceptor_begin_transaction(interceptor);
  gum_interceptor_attach(
      interceptor,
      GSIZE_TO_POINTER(gum_module_find_export_by_name(NULL, "open")), listener,
      GSIZE_TO_POINTER(EXAMPLE_HOOK_OPEN));
  gum_interceptor_attach(
      interceptor,
      GSIZE_TO_POINTER(gum_module_find_export_by_name(NULL, "close")), listener,
      GSIZE_TO_POINTER(EXAMPLE_HOOK_CLOSE));
  gum_interceptor_end_transaction(interceptor);

  __android_log_print(ANDROID_LOG_INFO, "adirfhook",
                      "[*] listener got %u calls\n",
                      EXAMPLE_LISTENER(listener)->num_calls);

  close(open("/proc/self/maps", O_RDONLY));
  close(open("/proc/self/maps", O_RDONLY));

  __android_log_print(ANDROID_LOG_INFO, "adirfhook",
                      "[*] listener got %u calls\n",
                      EXAMPLE_LISTENER(listener)->num_calls);

  gum_interceptor_detach(interceptor, listener);

  close(open("/proc/self/maps", O_RDONLY));
  close(open("/proc/self/maps", O_RDONLY));

  __android_log_print(ANDROID_LOG_INFO, "adirfhook",
                      "[*] listener still has %u calls\n",
                      EXAMPLE_LISTENER(listener)->num_calls);

  g_object_unref(listener);
  g_object_unref(interceptor);

  gum_deinit_embedded();

  return 0;
}

static void example_listener_on_enter(GumInvocationListener* listener,
                                      GumInvocationContext* ic) {
  ExampleListener* self = EXAMPLE_LISTENER(listener);
  ExampleHookId hook_id = (ExampleHookId)(GUM_IC_GET_FUNC_DATA(ic, gint64));
  __android_log_print(ANDROID_LOG_INFO, "adirfhook",
                      "[*] hook on entry, hook_id=%llu\n", (long long)hook_id);

  switch (hook_id) {
    case EXAMPLE_HOOK_OPEN:
      __android_log_print(
          ANDROID_LOG_INFO, "adirfhook", "[*] open(\"%s\")\n",
          (const gchar*)gum_invocation_context_get_nth_argument(ic, 0));
      break;
    case EXAMPLE_HOOK_CLOSE:
      __android_log_print(
          ANDROID_LOG_INFO, "adirfhook", "[*] close(%d)\n",
          GPOINTER_TO_INT(gum_invocation_context_get_nth_argument(ic, 0)));
      break;
  }

  self->num_calls++;
}

static void example_listener_on_leave(GumInvocationListener* listener,
                                      GumInvocationContext* ic) {}

static void example_listener_class_init(ExampleListenerClass* klass) {
  (void)EXAMPLE_IS_LISTENER;
  (void)glib_autoptr_cleanup_ExampleListener;
}

static void example_listener_iface_init(gpointer g_iface, gpointer iface_data) {
  GumInvocationListenerInterface* iface =
      (GumInvocationListenerInterface*)g_iface;

  iface->on_enter = example_listener_on_enter;
  iface->on_leave = example_listener_on_leave;
}

static void example_listener_init(ExampleListener* self) {}

JNIEXPORT jint __attribute__((visibility("default")))
JNI_OnLoad(JavaVM* vm, void* reserved) {
  __android_log_print(ANDROID_LOG_INFO, "adirfhook", "JNI_OnLoad");
  hook_test();
  JNIEnv* env = NULL;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return JNI_ERR;
  }

  return JNI_VERSION_1_6;
}
