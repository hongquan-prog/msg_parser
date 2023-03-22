#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include "msg_parser.h"

void decode_handler(void *param, uint8_t *data, int len)
{
    printf("msg_len:%d\r\n", len);

    for (int i = 0; i < len; i++)
    {
        if ((i % 10) == 0)
        {
            printf("\r\n");
        }

        printf("%02x ", data[i]);
    }
    printf("\r\n");
}

void main()
{
    uint8_t raw_data[120] = {0};
    msg_parser_t parser = {0};

    msg_t decode_msg = {0};
    uint8_t encode_len = 0;
    uint8_t *encode_data = NULL;
    msg_callback_t async_cb = {.func = decode_handler, .user_data = NULL};

    for (int i = 0; i < 120; i++)
    {
        raw_data[i] = i;
    }

    msg_register_decode_async_handler(&parser, &async_cb);
    msg_encode(&parser, raw_data, 120, &encode_data, &encode_len);

    /* 同步解析一次调用只能返回一个值，当一个数据包解析完成后，
       如果后面还存在下一帧的数据，只能由下一次调用来做解析，
       因为解析器一次只能解析一条数据，新的解析会覆盖旧的数据，
       在返回之后如果len不为0，则代表还有数据未解析完，需要进行第二次解析
     */
    while (encode_len)
    {
        if (msg_decode(&parser, &decode_msg, encode_data, &encode_len))
        {
            decode_handler(NULL, decode_msg.payload, decode_msg.data_length);
        }
    }

    /* 异步解析，每次解析完成后自动调用回调函数，如果发现还有未处理的数据
       会在回调函数处理完成之后进行第二次解析。
     */
    msg_encode(&parser, raw_data, 120, &encode_data, &encode_len);
    msg_decode_async(&parser, encode_data, encode_len);
}
