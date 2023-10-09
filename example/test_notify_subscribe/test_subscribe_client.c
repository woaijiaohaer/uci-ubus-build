#include <unistd.h>
#include <libubox/blobmsg_json.h>
#include <libubox/uloop.h>
#include <libubus.h>

static struct ubus_context *ubus_ctx;
static int counter = 0;
static uint32_t obj_id = 0;
static struct ubus_subscriber test_subscriber;

static int test_notify_message_deal_cb(struct ubus_context *ctx, struct ubus_object *obj,
                                       struct ubus_request_data *req, const char *method, struct blob_attr *msg)
{
    char *str = NULL;
    printf("notify handler...method=%s\n", method);

    str = blobmsg_format_json(msg, true);
    printf("{ \"%s\": %s }\n", method, str);
    free(str);

    counter++;

    if (counter > 3)
    {
        ubus_unsubscribe(ctx, &test_subscriber, obj_id);
    }
    return 0;
}

static void test_object_remove_cb(struct ubus_context *ctx,
                                  struct ubus_subscriber *obj, uint32_t id)
{
    printf("remove handler...\n");
}

static void test_subscriber_object(void)
{
    int ret = 0;

    test_subscriber.cb = test_notify_message_deal_cb;
    test_subscriber.remove_cb = test_object_remove_cb;

    ret = ubus_register_subscriber(ubus_ctx, &test_subscriber);
    if (ret)
    {
        printf("[%s][%d]Error happen : %s \n", __FUNCTION__, __LINE__, ubus_strerror(ret));
        return ;
    }

    ret = ubus_lookup_id(ubus_ctx, "test", &obj_id);
    if (ret)
    {
        printf("[%s][%d]Error happen : %s \n", __FUNCTION__, __LINE__, ubus_strerror(ret));
        return ;
    }


    ret = ubus_subscribe(ubus_ctx, &test_subscriber, obj_id);
    if (ret)
    {
        printf("[%s][%d]Error happen : %s \n", __FUNCTION__, __LINE__, ubus_strerror(ret));
        return ;
    }

    return;
}

int main(int argc, char **argv)
{
    uloop_init();

    ubus_ctx = ubus_connect(NULL);
    if (NULL == ubus_ctx)
    {
        printf("[%s][%d]error Happen.\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ubus_add_uloop(ubus_ctx);

    test_subscriber_object();

    uloop_run();

    ubus_free(ubus_ctx);
    uloop_done();

    return 0;
}
