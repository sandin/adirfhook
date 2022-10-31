#include "adirfhook/hook/hookmanager.h"

#include <android/log.h>

using namespace adirfhook;

typedef struct _ExampleListener ExampleListener;

struct _ExampleListener {
  GObject parent;

  void* mgr;

  guint num_calls;
};


enum { PROP_MGR = 1, N_PROPERTIES };

static GParamSpec* obj_properties[N_PROPERTIES] = {
    NULL
};

static void example_listener_iface_init(gpointer g_iface, gpointer iface_data);

#define EXAMPLE_TYPE_LISTENER (example_listener_get_type())
G_DECLARE_FINAL_TYPE(ExampleListener, example_listener, EXAMPLE, LISTENER,
                     GObject)
G_DEFINE_TYPE_EXTENDED(ExampleListener, example_listener, G_TYPE_OBJECT, 0,
                       G_IMPLEMENT_INTERFACE(GUM_TYPE_INVOCATION_LISTENER,
                                             example_listener_iface_init))

static void example_listener_set_property(GObject* object, guint prop_id,
                                          const GValue* value,
                                          GParamSpec* pspec) {
  __android_log_print(ANDROID_LOG_INFO, "adirfhook", "example_listener_set_property"); 
  ExampleListener* obj = EXAMPLE_LISTENER(object);

  switch (prop_id) {
    case PROP_MGR:
      obj->mgr = g_value_get_pointer(value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void example_listener_get_property(GObject* object, guint prop_id,
                                          GValue* value, GParamSpec* pspec) {
  __android_log_print(ANDROID_LOG_INFO, "adirfhook", "example_listener_get_property"); 
  ExampleListener* obj = EXAMPLE_LISTENER(object);

  switch (prop_id) {
    case PROP_MGR:
      g_value_set_pointer(value, obj->mgr);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void example_listener_class_init(ExampleListenerClass* klass) {
  __android_log_print(ANDROID_LOG_INFO, "adirfhook", "example_listener_class_init"); 
  /*
  (void)EXAMPLE_IS_LISTENER;
  (void)glib_autoptr_cleanup_ExampleListener;
  */

  GObjectClass* object_class = G_OBJECT_CLASS(klass);

  object_class->set_property = example_listener_set_property;
  object_class->get_property = example_listener_get_property;

  obj_properties[PROP_MGR] = g_param_spec_string("manager", "manager", "HookManager.", NULL,
      (GParamFlags)(G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));

  g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);
}

static void example_listener_init(ExampleListener* self) {}

static void example_listener_on_enter(GumInvocationListener* listener,
                                      GumInvocationContext* ic) {
  __android_log_print(ANDROID_LOG_INFO, "adirfhook", "hook on enter"); 
  // TODO
}

static void example_listener_on_leave(GumInvocationListener* listener,
                                      GumInvocationContext* ic) {
  // TODO
}

static void example_listener_iface_init(gpointer g_iface, gpointer iface_data) {
  GumInvocationListenerInterface* iface =
      (GumInvocationListenerInterface*)g_iface;

  iface->on_enter = example_listener_on_enter;
  iface->on_leave = example_listener_on_leave;
}

HookManager::HookManager() {
  gum_init_embedded();

  interceptor_ = gum_interceptor_obtain();
  default_listener_ = (GumInvocationListener*)g_object_new(EXAMPLE_TYPE_LISTENER, "manager", NULL, NULL);
  //default_listener_ = (GumInvocationListener*)g_object_new(EXAMPLE_TYPE_LISTENER, NULL);
}

HookManager::~HookManager() {
  gum_interceptor_detach(interceptor_, default_listener_);

  g_object_unref(default_listener_);
  g_object_unref(interceptor_);

  gum_deinit_embedded();
}

HookManager::HookId HookManager::Hook(void* function_addr,
                                      HookCallback callback) {
  uint64_t hook_id = next_hook_id_.fetch_add(1, std::memory_order_relaxed);
  HookContext* hook_context = new HookContext(hook_id, function_addr);
  hook_contexts_.insert(std::make_pair(hook_id, hook_context));

  gum_interceptor_begin_transaction(interceptor_);
  gum_interceptor_attach(interceptor_, static_cast<gpointer>(function_addr),
                         default_listener_, hook_context);
  gum_interceptor_end_transaction(interceptor_);

  return hook_id;
}

bool HookManager::Unhook(HookManager::HookId hook_id) {
  auto found = hook_contexts_.find(hook_id);
  if (found != hook_contexts_.end()) {
    HookContext* hook_context = found->second;
    gum_interceptor_revert(interceptor_, hook_context->HookTarget());
    delete hook_context;
    hook_contexts_.erase(found);
    return true;
  }

  return false;
}