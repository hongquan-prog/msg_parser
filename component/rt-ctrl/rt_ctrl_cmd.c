#include "rt_ctrl_cmd.h"
#include <stddef.h>

typedef struct
{
    rt_ctrl_cmd_def cmd;
    rt_ctrl_cmd_callback_t cb;
} rt_ctrl_callback_node_t;

rt_ctrl_callback_node_t prv_callback_list[] = {
    {RT_CTRL_CMD_PING},
    {RT_CTRL_CMD_HOMING},
    {RT_CTRL_CMD_MOVING},
    {RT_CTRL_CMD_SORT},
    {RT_CTRL_CMD_MOVING_CANCEL},
    {RT_CTRL_CMD_SENSOR_STATE},
    {RT_CTRL_CMD_PICK_BOX},
    {RT_CTRL_CMD_PICK_PIN},
    {RT_CTRL_CMD_PICK_CHAIN_HOMING},
    {RT_CTRL_CMD_PICK_SENSOR_STATE},
    {RT_CTRL_CMD_MOVING_READ},
    {RT_CTRL_CMD_PICK_CHAIN_DIR_CTRL},
    {RT_CTRL_CMD_HALT},
    {RT_CTRL_CMD_PICK_SENSOR_CHECK_LOADED},
    {RT_CTRL_CMD_PICK_SENSOR_CHECK_BEFORE_LOADING},
    {RT_CTRL_CMD_PICK_SENSOR_CHECK_UNLOADED},
    {RT_CTRL_CMD_DEBUG},
    {RT_CTRL_CMD_CONFIG}};

static int rt_ctrl_cmd_index(rt_ctrl_cmd_def cmd)
{
    int ret = -1;
    int count = sizeof(prv_callback_list) / sizeof(rt_ctrl_callback_node_t);

    for (int i = 0; i < count; i++)
    {
        if (cmd == prv_callback_list[i].cmd)
        {
            ret = i;
            break;
        }
    }

    return ret;
}

void rt_ctrl_msg_handler(uint8_t *data, int len)
{
    int ret = -1;

    if ((NULL == data) || (len <= 0))
    {
        return;
    }

    ret = rt_ctrl_cmd_index(data[0]);

    if (-1 != ret && prv_callback_list[ret].cb.func)
    {
        prv_callback_list[ret].cb.func(prv_callback_list[ret].cb.user_data, data + 1, len - 1);
    }
}

void rt_ctrl_register_cmd_callback(rt_ctrl_cmd_def cmd, rt_ctrl_cmd_callback_t *cb)
{
    int ret = -1;

    if (NULL == cb)
    {
        return;
    }

    ret = rt_ctrl_cmd_index(cmd);

    if (-1 != ret)
    {
        prv_callback_list[ret].cb = *cb;
    }
}