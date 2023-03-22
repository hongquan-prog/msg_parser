#pragma once

#include <stdint.h>

typedef enum
{
    RT_CTRL_CMD_PING,
    RT_CTRL_CMD_HOMING,
    RT_CTRL_CMD_MOVING,
    RT_CTRL_CMD_SORT,
    RT_CTRL_CMD_MOVING_CANCEL,
    RT_CTRL_CMD_SENSOR_STATE,
    RT_CTRL_CMD_PICK_BOX,
    RT_CTRL_CMD_PICK_PIN,
    RT_CTRL_CMD_PICK_CHAIN_HOMING,
    RT_CTRL_CMD_PICK_SENSOR_STATE,
    RT_CTRL_CMD_MOVING_READ,
    RT_CTRL_CMD_PICK_CHAIN_DIR_CTRL,
    RT_CTRL_CMD_HALT,
    RT_CTRL_CMD_PICK_SENSOR_CHECK_LOADED,
    RT_CTRL_CMD_PICK_SENSOR_CHECK_BEFORE_LOADING,
    RT_CTRL_CMD_PICK_SENSOR_CHECK_UNLOADED,
    RT_CTRL_CMD_DEBUG,
    RT_CTRL_CMD_CONFIG
} rt_ctrl_cmd_def;

typedef struct
{
    void *user_data;
    void (*func)(void *user_data, uint8_t *data, int len);
} rt_ctrl_cmd_callback_t;

void rt_ctrl_msg_handler(uint8_t *data, int len);
void rt_ctrl_register_cmd_callback(rt_ctrl_cmd_def cmd, rt_ctrl_cmd_callback_t *cb);