#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libubus.h>
#include <libubox/uloop.h>
#include <libubox/blobmsg_json.h>

static struct ubus_context *ubus = NULL;
static struct ubus_subscriber processB_subscriber;
static uint32_t objid_processB = 0;
static uint32_t objid_processA = 0;
static struct blob_buf b;

enum
{
  PB_NOTIFY_CMD,
  __PB_NOTIFY_MAX,
};

static const struct blobmsg_policy pb_cmd_policy[__PB_NOTIFY_MAX] = {
    [PB_NOTIFY_CMD] = {.name = "command", .type = BLOBMSG_TYPE_INT8},
};

static int handle_processB_notify(struct blob_attr *msg)
{
  printf("A: handle processB notify\n");

  uint8_t command = 0;

  struct blob_attr *tb[__PB_NOTIFY_MAX], *c;
  blobmsg_parse(pb_cmd_policy, __PB_NOTIFY_MAX, tb, blob_data(msg), blob_len(msg));

  if ((c = tb[PB_NOTIFY_CMD]))
  {
    // acqurie results from Process B
    command = blobmsg_get_u8(c);
  }

  return 0;
}

enum
{
  PA_CALL_ENABLE,
  __PA_CALL_MAX
};

static const struct blobmsg_policy pa_call_policy[__PA_CALL_MAX] = {
    [PA_CALL_ENABLE] = {.name = "enable", .type = BLOBMSG_TYPE_BOOL},
};

// method for ubus call
static int handle_pa_call(struct ubus_context *ctx, struct ubus_object *obj,
                          struct ubus_request_data *req, const char *method,
                          struct blob_attr *msg)
{

  struct blob_attr *tb[__PA_CALL_MAX], *c;

  bool enable = false;

  blobmsg_parse(pa_call_policy, __PA_CALL_MAX, tb, blob_data(msg), blob_len(msg));

  if ((c = tb[PA_CALL_ENABLE]))
  {
    enable = blobmsg_get_u8(c);
  }

  if (enable)
  {
    // call Process B to do something...
    blob_buf_init(&b, 0);
    blobmsg_add_u32(&b, "command", enable);
    ubus_invoke(ubus, objid_processB, "pb_call_method", b.head, NULL, NULL, 2000);
  }

  return 0;
}

static void update_processB(bool subscribe)
{
  if (subscribe)
  {
    ubus_subscribe(ubus, &processB_subscriber, objid_processB);
    printf("A: ubus subscribe objid_processB\n");
  }
}

// create a method for ubus
static struct ubus_method processA_object_methods[] = {
    UBUS_METHOD("pa_call_command", handle_pa_call, pa_call_policy),
};

static struct ubus_object_type processA_object_type =
    UBUS_OBJECT_TYPE("processA", processA_object_methods);

static struct ubus_object processA_object = {
    .name = "processA",
    .type = &processA_object_type,
    .methods = processA_object_methods,
    .n_methods = ARRAY_SIZE(processA_object_methods),
};

static int processB_notify(struct ubus_context *ctx, struct ubus_object *obj,
                           struct ubus_request_data *req, const char *method,
                           struct blob_attr *msg)
{

  if (strcmp(method, "pb_notify") == 0)
  {
    // receive results from Process B
    handle_processB_notify(msg);
  }
  return 0;
}

int init_ubus(void)
{

  // initialize ubus of Process A
  uloop_init();
  printf("A: uloop init\n");

  // connect to ubus server
  if (!(ubus = ubus_connect(NULL)))
  {
    return -1;
  }

  // set up subscribing Process B object
  processB_subscriber.cb = processB_notify;
  ubus_register_subscriber(ubus, &processB_subscriber);
  printf("A: ubus register subscriber\n");

  ubus_add_uloop(ubus);

  // create ubus method
  ubus_add_object(ubus, &processA_object);

  if (!ubus_lookup_id(ubus, "processB", &objid_processB))
  {
    update_processB(true);

    blob_buf_init(&b, 0);
    blobmsg_add_u32(&b, "command", true);
    ubus_invoke(ubus, objid_processB, "pb_call_method", b.head, NULL, NULL, 2000);
  }

  uloop_run();

  return 0;
}

int exit_ubus(void)
{

  ubus_free(ubus);
  uloop_done();
  return 0;
}

int main()
{
  init_ubus();

  return 0;
}