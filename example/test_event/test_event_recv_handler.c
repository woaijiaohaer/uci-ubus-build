#include <libubox/uloop.h>
#include <libubox/ustream.h>
#include <libubox/utils.h>
#include <libubus.h>
#include <libubox/blobmsg_json.h>
#include "test_event.h"

static struct ubus_context *ubus_ctx = NULL;
static struct ubus_event_handler enent_listener;


static void test_ubus_recv_event_cb(struct ubus_context *ctx, struct ubus_event_handler *ev,
                                    const char *type, struct blob_attr *msg)
{
    char *p = NULL;

    if (strcmp(type, TEST_EVENT_ACTION_1) == 0)
    {
        p = blobmsg_format_json(msg, true);
        printf("Recv:{ \"%s\": %s }\n", type, p);
        free(p);
    }
    else if (strcmp(type, TEST_EVENT_ACTION_2) == 0)
    {
        p = blobmsg_format_json(msg, true);
        printf("Recv:{ \"%s\": %s }\n", type, p);
        free(p);
    }
    else
    {
        printf("[%s][%d]Recv:Unkonw type/path.\n", __FUNCTION__, __LINE__);
    }
}

static int test_ubus_register_events(void)
{
    int ret = 0;

    memset(&enent_listener, 0, sizeof(enent_listener));
    enent_listener.cb = test_ubus_recv_event_cb;

    ret = ubus_register_event_handler(ubus_ctx, &enent_listener, TEST_EVENT_ACTION_1);
    if (ret)
    {
        printf("[%s][%d]Error happen.\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ret = ubus_register_event_handler(ubus_ctx, &enent_listener, TEST_EVENT_ACTION_2);
    if (ret)
    {
        ubus_unregister_event_handler(ubus_ctx, &enent_listener);
        printf("[%s][%d]Error happen.\n", __FUNCTION__, __LINE__);
        return -1;
    }

    return 0;
}



int main(int argc, char *argv[])
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
    test_ubus_register_events();

    uloop_run();

    /* Free action. */
    if(ubus_ctx)
    {
        ubus_free(ubus_ctx);
    }

    uloop_done();

    return 0;
}
