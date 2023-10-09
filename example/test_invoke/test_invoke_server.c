#include <libubox/uloop.h>
#include <libubox/ustream.h>
#include <libubox/utils.h>
#include <libubus.h>
#include <libubox/blobmsg_json.h>
#include "test_invoke.h"

static struct ubus_context *ubus_ctx = NULL;
static struct blob_buf b_buf;
static struct pri_info g_msg = {"default", 255};

enum
{
    TEST_INVOKE_NAME,
    TEST_INVOKE_ID,
    TEST_INVOKE_MAX
};

static int test_notify_func();


static const struct blobmsg_policy test_invoke_get_policy[TEST_INVOKE_MAX] =
{
    [TEST_INVOKE_NAME] = {.name = "name", .type = BLOBMSG_TYPE_STRING},
};

static const struct blobmsg_policy test_invoke_set_policy[TEST_INVOKE_MAX] =
{
    [TEST_INVOKE_NAME] = {.name = "name", .type = BLOBMSG_TYPE_STRING},
    [TEST_INVOKE_ID] = {.name = "id", .type = BLOBMSG_TYPE_INT32},
};


static int test_invoke_get_cb(struct ubus_context *ctx, struct ubus_object *obj,
                              struct ubus_request_data *req, const char *method,
                              struct blob_attr *msg)
{
    struct blob_attr *tb[TEST_INVOKE_MAX];
    char result[64] = "Good.";
    char *name = NULL;

    blob_buf_init(&b_buf, 0);

    blobmsg_parse(test_invoke_get_policy, TEST_INVOKE_MAX, tb, blob_data(msg), blob_len(msg));
    if(NULL == tb[TEST_INVOKE_NAME])
    {
        printf("[%s][%d] Need name.\n", __FUNCTION__, __LINE__);
        snprintf(result, sizeof(result), "%s", "Need name.");
        goto out;
    }

    name = blobmsg_get_string(tb[TEST_INVOKE_NAME]);
    if(0 != strcmp(name, g_msg.name))
    {
        printf("[%s][%d] Need name.\n", __FUNCTION__, __LINE__);
        snprintf(result, sizeof(result), "%s", "Name not match.");
        goto out;
    }

    blobmsg_add_string(&b_buf, "name", g_msg.name);
    blobmsg_add_u32(&b_buf, "id", g_msg.id);

out:
    blobmsg_add_string(&b_buf, "result", result);
    ubus_send_reply(ctx, req, b_buf.head);
    test_notify_func();

    return 0;
}



static int test_invoke_set_cb(struct ubus_context *ctx, struct ubus_object *obj,
                              struct ubus_request_data *req, const char *method,
                              struct blob_attr *msg)
{
    struct blob_attr *tb[TEST_INVOKE_MAX];
    char result[64] = "Good.";
    char *name = NULL;
    int id = 0;


    blob_buf_init(&b_buf, 0);

    blobmsg_parse(test_invoke_set_policy, TEST_INVOKE_MAX, tb, blob_data(msg), blob_len(msg));

    if(NULL == tb[TEST_INVOKE_NAME] || NULL == tb[TEST_INVOKE_ID])
    {
        printf("[%s][%d] Need name.\n", __FUNCTION__, __LINE__);
        snprintf(result, sizeof(result), "%s", "Parameter lost.");
        goto out;
    }

    name = blobmsg_get_string(tb[TEST_INVOKE_NAME]);
    id = blobmsg_get_u32(tb[TEST_INVOKE_ID]);

    printf("[%s][%d]name=%s, id=%d\n", __FUNCTION__, __LINE__, name, id);
    if(name)
    {
        snprintf(g_msg.name, sizeof(g_msg.name), "%s", name);
    }
    else
    {
        printf("[%s][%d]Name error.\n", __FUNCTION__, __LINE__);
    }
    g_msg.id = id;



out:
    blobmsg_add_string(&b_buf, "result", result);
    ubus_send_reply(ctx, req, b_buf.head);
    test_notify_func();  // Notify subscribers that someone Invoke this object. Just testing.

    return 0;
}


static struct ubus_method test_invoke_methods[] =
{
    UBUS_METHOD("get", test_invoke_get_cb, test_invoke_get_policy),
    UBUS_METHOD("set", test_invoke_set_cb, test_invoke_set_policy),
};

static struct ubus_object_type test_invoke_obj_type
    = UBUS_OBJECT_TYPE("test_invoke", test_invoke_methods);

static struct ubus_object test_invoke_obj =
{
    .name = "test_invoke",
    .type = &test_invoke_obj_type,
    .methods = test_invoke_methods,
    .n_methods = ARRAY_SIZE(test_invoke_methods),
};

static int test_ubus_add_object()
{

    if (ubus_add_object(ubus_ctx, &test_invoke_obj) != 0)
    {
        printf("[%s][%d]Add object Failed.\n", __FUNCTION__, __LINE__);
        return -1;
    }

    printf("[%s][%d] Add object Success.\n", __FUNCTION__, __LINE__);
    return 0;
}

static int test_notify_func()
{
    blob_buf_init(&b_buf, 0);
    blobmsg_add_string(&b_buf, "info", "test_invoke is call now.");
    ubus_notify(ubus_ctx,  &test_invoke_obj, "call", b_buf.head, -1);

    return 0;
}


int main(int argc, char *argv[])
{
    uloop_init();

    ubus_ctx = ubus_connect(NULL);
    if(NULL == ubus_ctx)
    {
        printf("[%s][%d]Error happen.\n", __FUNCTION__, __LINE__);
        return -1;
    }
    ubus_add_uloop(ubus_ctx);

    test_ubus_add_object();

    uloop_run();

    if(ubus_ctx)
    {
        ubus_free(ubus_ctx);
    }
    uloop_done();

    return 0;
}
