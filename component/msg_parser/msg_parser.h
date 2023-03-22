#pragma once

#include <stdint.h>

#define MSG_BUFFER_SIZE 128
#define MSG_START 0xAA

typedef struct
{
    uint8_t data_length;
    uint8_t *payload;
} msg_t;

typedef enum
{
    MSG_ERR_NONE,
    MSG_ERR_ENCODE_OVERFLOW,
    MSG_ERR_DECODE_OVERFLOW,
    MSG_ERR_ENCODE_INPUT_INVALID
} msg_status_def;

#pragma pack(1)
typedef struct
{
    uint8_t seq_num;
    uint8_t data_length;
} msg_packet_header_t;

typedef struct
{
    uint8_t crc;
} msg_packet_tail_t;

typedef struct
{
    msg_packet_header_t header;
    uint8_t *payload;
    msg_packet_tail_t tail;
} msg_packet_t;
#pragma pack()

typedef struct
{
    void *user_data;
    void (*func)(void *user_data, uint8_t *data, int len);
} msg_callback_t;

typedef struct
{
    /* encoder */
    uint8_t encode_offset;
    uint8_t encode_buffer[MSG_BUFFER_SIZE];
    uint8_t seq;

    msg_callback_t decoder_cb;
    /* decoder */
    uint8_t decoder_init;
    uint8_t decode_buffer[MSG_BUFFER_SIZE];
    /* decoder cache */
    msg_packet_t decode_cache;
    msg_packet_t *decode_msg;
    /* decoder status */
    uint8_t recved_start;
    uint8_t recved_header;
    uint8_t remain;
    /* if decode_overflow is not equal to 0, decoder will discard the data until remain redece to 0 */
    uint8_t decode_overflow;
} msg_parser_t;

void msg_register_decode_async_handler(msg_parser_t *parser, msg_callback_t *cb);
msg_status_def msg_encode(msg_parser_t *parser, uint8_t *data, uint8_t len, uint8_t **out, uint8_t *out_len);
msg_t *msg_decode(msg_parser_t *parser, msg_t *msg, uint8_t *data, uint8_t *len);
void msg_decode_async(msg_parser_t *parser, uint8_t *data, uint8_t len);
