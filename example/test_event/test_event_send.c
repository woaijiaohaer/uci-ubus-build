/*
    Ubus event communition test.

    Send An event to Hander Center.

*/
#include <libubox/uloop.h>
#include <libubox/ustream.h>
#include <libubox/utils.h>
#include <libubus.h>
#include <libubox/blobmsg_json.h>

#include "test_event.h"

static struct ubus_context *ubus_ctx = NULL;
static struct blob_buf ubus_b;

static int test_ubus_send_event(void)
{
    int ret = 0;
    char *p = NULL;
    void *table = NULL;

    /* Add blob message. */
    blob_buf_init(&ubus_b, 0);

    blobmsg_add_string(&ubus_b, "name", "action_1");
    blobmsg_add_u32(&ubus_b, "max", 100);
    blobmsg_add_u32(&ubus_b, "min", 1);

    table = blobmsg_open_table(&ubus_b, "object");
    blobmsg_add_string(&ubus_b, "object_test", "true");
    blobmsg_close_table(&ubus_b, table);

    /* Send an event to special PATH. */
    ret  = ubus_send_event(ubus_ctx, TEST_EVENT_ACTION_1, ubus_b.head);
    if(UBUS_STATUS_OK != ret)
    {
        printf("[%s][%d] Send error happen.\n", __FUNCTION__, __LINE__);
    }
    else
    {
        p = blobmsg_format_json_indent(ubus_b.head, 1, 1);
        printf("[%s][%d] Send message OK.\n[%s]\n", __FUNCTION__, __LINE__, p);
        if(p)
        {
            free(p);
        }
    }


    /* Add blob message. */
    blob_buf_init(&ubus_b, 0);

    blobmsg_add_string(&ubus_b, "name", "action_2");
    blobmsg_add_u32(&ubus_b, "max", 100);
    blobmsg_add_u32(&ubus_b, "min", 1);

    table = blobmsg_open_table(&ubus_b, "object");
    blobmsg_add_string(&ubus_b, "object_test", "true");
    blobmsg_close_table(&ubus_b, table);

    /* Send an event to special PATH. */
    ret  = ubus_send_event(ubus_ctx, TEST_EVENT_ACTION_2, ubus_b.head);
    if(UBUS_STATUS_OK != ret)
    {
        printf("[%s][%d] Send error happen.\n", __FUNCTION__, __LINE__);
    }
    else
    {
        p = blobmsg_format_json_indent(ubus_b.head, 1, 1);
        printf("[%s][%d] Send message OK.\n[%s]\n", __FUNCTION__, __LINE__, p);
        if(p)
        {
            free(p);
        }
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
    test_ubus_send_event();


    //uloop_run(); No need , Just exit.
    
    /* Free action. */
    if(ubus_ctx)
    {
        ubus_free(ubus_ctx);
    }

    uloop_done();

    return 0;
}
