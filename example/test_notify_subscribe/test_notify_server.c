#include <unistd.h>
#include <libubox/blobmsg_json.h>
#include <libubox/uloop.h>
#include <libubox/ustream.h>
#include <libubox/utils.h>
#include <libubus.h>

static void test_timer(struct uloop_timeout *t);
static void test_client_subscribe_cb(struct ubus_context *ctx, struct ubus_object *obj);


static struct ubus_context *ubus_ctx = NULL;
static struct uloop_timeout timer = {.cb = test_timer};
static struct blob_buf b_buf;


static struct ubus_method test_methods[] = {};

static struct ubus_object_type test_obj_type =
    UBUS_OBJECT_TYPE("test", test_methods);

static struct ubus_object test_object = {
    .name = "test",
    .type = &test_obj_type,
    .subscribe_cb = test_client_subscribe_cb,
};

static void test_timer(struct uloop_timeout *t)
{
    blob_buf_init(&b_buf,  0);

    blobmsg_add_string(&b_buf, "name", "wxy");

    ubus_notify(ubus_ctx,  &test_object, "test_type", b_buf.head, -1); // -1 For no response.

    uloop_timeout_set(t, 2 * 1000);
}

static void test_client_subscribe_cb(struct ubus_context *ctx, struct ubus_object *obj)
{
    printf("[%s][%d]Subscribers active: %d.\n", __FUNCTION__, __LINE__, obj->has_subscribers);
}


static void test_notifier_add_object(void)
{
    int ret = 0;

    ret = ubus_add_object(ubus_ctx, &test_object);
    if (ret)
    {
        printf("[%s][%d]Error happen.\n", __FUNCTION__, __LINE__);
        return;
    }

    printf("[%s][%d] Add object Success.\n", __FUNCTION__, __LINE__);
    return;
}

int main(int argc, char **argv)
{
    /* Initialize action. */
    uloop_init();

    ubus_ctx = ubus_connect(NULL);
    if(NULL == ubus_ctx)
    {
        printf("[%s][%d]Error happen.\n", __FUNCTION__, __LINE__);
        return -1;
    }
    ubus_add_uloop(ubus_ctx);

    /* Some testing. */
    test_notifier_add_object();

    uloop_timeout_set(&timer, 2 * 1000);

    uloop_run();

    /* Free action. */
    if(ubus_ctx)
    {
        ubus_free(ubus_ctx);
    }

    uloop_done();

    return 0;

}
