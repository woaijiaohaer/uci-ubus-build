#include <libubox/uloop.h>
#include <libubox/ustream.h>
#include <libubox/utils.h>
#include <libubus.h>
#include <libubox/blobmsg_json.h>
#include "test_invoke.h"

static struct ubus_context *ubus_ctx = NULL;
static struct blob_buf b_buf;
static struct pri_info g_msg = {"test_1", 221};

enum
{
    TEST_INVOKE_NAME,
    TEST_INVOKE_ID,
    TEST_INVOKE_MAX
};


static const struct blobmsg_policy test_invoke_policy[TEST_INVOKE_MAX] =
{
    [TEST_INVOKE_NAME] = {.name = "name", .type = BLOBMSG_TYPE_STRING},
    [TEST_INVOKE_ID] = {.name = "id", .type = BLOBMSG_TYPE_INT32},
};


static void test_invoke_client_get_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
    struct blob_attr *tb[TEST_INVOKE_MAX];

    char *name = NULL;
    int id = 0;
    char *p = NULL;

    p = blobmsg_format_json(msg, 1);
    if(p)
    {
        printf("[%s][%d] Recv:%s\n", __FUNCTION__, __LINE__, p);
        free(p);
    }
    else
    {
        printf("[%s][%d]blobmsg_format_json failed.\n", __FUNCTION__, __LINE__);
    }

    blobmsg_parse(test_invoke_policy, TEST_INVOKE_MAX, tb, blob_data(msg), blob_len(msg));
    if(NULL == tb[TEST_INVOKE_NAME] || NULL == tb[TEST_INVOKE_ID])
    {
        printf("[%s][%d] Message not contain.\n", __FUNCTION__, __LINE__);
        return;
    }

    name = blobmsg_get_string( tb[TEST_INVOKE_NAME]);
    id = blobmsg_get_u32(tb[TEST_INVOKE_ID]);

    printf("[%s][%d]Get name:%s, id:%d\n", __FUNCTION__, __LINE__, name, id);
    return ;
}

static void test_invoke_client_set_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{

    char *p = NULL;

    p = blobmsg_format_json(msg, 1);
    if(p)
    {
        printf("[%s][%d] Recv:%s\n", __FUNCTION__, __LINE__, p);
        free(p);
    }
    else
    {
        printf("[%s][%d]blobmsg_format_json failed.\n", __FUNCTION__, __LINE__);
    }
    return ;
}


static int test_invoke_client_call()
{
    unsigned int id;
    int ret;


    ret = ubus_lookup_id(ubus_ctx, "test_invoke", &id);
    if (UBUS_STATUS_OK != ret )
    {
        printf("[%s][%d]ubus_lookup_id Failed.\n", __FUNCTION__, __LINE__);
        return ret;
    }
    else
    {
        printf("[%s][%d]ubus_lookup_id Success.\n", __FUNCTION__, __LINE__);
    }

    /* Get action. */
    blob_buf_init(&b_buf, 0);
    blobmsg_add_string(&b_buf, test_invoke_policy[TEST_INVOKE_NAME].name, "default");
    ret = ubus_invoke(ubus_ctx, id, "get", b_buf.head, test_invoke_client_get_cb, NULL, 2 * 1000);
    if (UBUS_STATUS_OK != ret )
    {
        printf("[%s][%d]ubus_invoke get Failed.\n", __FUNCTION__, __LINE__);
        return ret;
    }
    else
    {
        printf("[%s][%d]ubus_invoke get Success.\n", __FUNCTION__, __LINE__);
    }

    /* Set action. */
    blob_buf_init(&b_buf, 0);
    blobmsg_add_string(&b_buf, test_invoke_policy[TEST_INVOKE_NAME].name, g_msg.name);
    blobmsg_add_u32(&b_buf, test_invoke_policy[TEST_INVOKE_ID].name, g_msg.id);
    ret = ubus_invoke(ubus_ctx, id, "set", b_buf.head, test_invoke_client_set_cb, NULL, 2 * 1000);
    if (UBUS_STATUS_OK != ret )
    {
        printf("[%s][%d]ubus_invoke set Failed.\n", __FUNCTION__, __LINE__);
        return ret;
    }
    else
    {
        printf("[%s][%d]ubus_invoke set Success.\n", __FUNCTION__, __LINE__);
    }

    /* Check it. */
    blob_buf_init(&b_buf, 0);
    blobmsg_add_string(&b_buf, test_invoke_policy[TEST_INVOKE_NAME].name, g_msg.name);
    ret = ubus_invoke(ubus_ctx, id, "get", b_buf.head, test_invoke_client_get_cb, NULL, 2 * 1000);
    if (UBUS_STATUS_OK != ret )
    {
        printf("[%s][%d]ubus_invoke get Failed.\n", __FUNCTION__, __LINE__);
        return ret;
    }
    else
    {
        printf("[%s][%d]ubus_invoke get Success.\n", __FUNCTION__, __LINE__);
    }


    return 0;
}


int main(int argc, char *argv[])
{
    uloop_init();
    ubus_ctx = ubus_connect(NULL);
    if(NULL == ubus_ctx)
    {
        printf("[%s][%d]error Happen\n", __FUNCTION__, __LINE__);
        return -1;
    }
    ubus_add_uloop(ubus_ctx);

    test_invoke_client_call();
    //uloop_run();
    ubus_free(ubus_ctx);
    uloop_done();

    return 0;
}
