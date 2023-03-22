#include "msg_parser.h"
#include "crc.h"
#include <string.h>

static void msg_encoder_init(msg_parser_t *parser)
{
    parser->encode_buffer[0] = MSG_START;
    parser->encode_offset = 1;
}

static msg_status_def msg_encode_append(msg_parser_t *parser, uint8_t *data, uint8_t len)
{
    msg_status_def ret = MSG_ERR_NONE;

    if ((parser->encode_offset + len) < MSG_BUFFER_SIZE)
    {
        memcpy(parser->encode_buffer + parser->encode_offset, data, len);
        parser->encode_offset += len;
    }
    else
    {
        ret = MSG_ERR_ENCODE_OVERFLOW;
    }

    return ret;
}

static msg_status_def msg_encode_fihish(msg_parser_t *parser)
{
    msg_status_def ret = MSG_ERR_NONE;
    return ret;
}

static msg_status_def msg_encode_packet(msg_parser_t *parser, msg_packet_t *pack)
{
    msg_status_def ret = MSG_ERR_NONE;

    /* add start byte */
    msg_encoder_init(parser);
    /* add header*/
    (MSG_ERR_NONE == ret) ? (ret = msg_encode_append(parser, (uint8_t *)&pack->header, sizeof(msg_packet_header_t))) : (0);
    /* add payload*/
    (MSG_ERR_NONE == ret) ? (ret = msg_encode_append(parser, pack->payload, pack->header.data_length)) : (0);
    /* add tail*/
    (MSG_ERR_NONE == ret) ? (ret = msg_encode_append(parser, (uint8_t *)&pack->tail, sizeof(msg_packet_tail_t))) : (0);
    /* add end byte*/
    (MSG_ERR_NONE == ret) ? (ret = msg_encode_fihish(parser)) : (0);

    return ret;
}

msg_status_def msg_encode(msg_parser_t *parser, uint8_t *data, uint8_t len, uint8_t **out, uint8_t *out_len)
{
    msg_packet_t pack = {0};
    msg_status_def ret = MSG_ERR_NONE;

    if (!data || !len)
    {
        return MSG_ERR_ENCODE_INPUT_INVALID;
    }

    pack.header.seq_num = parser->seq++;
    pack.header.data_length = len;
    pack.payload = data;
    /* calculate crc */
    pack.tail.crc = crc8(0, (uint8_t *)&(pack.header), sizeof(msg_packet_header_t));
    pack.tail.crc = crc8(pack.tail.crc, pack.payload, pack.header.data_length);
    /* encode packet into buffer */
    ret = msg_encode_packet(parser, &pack);

    if (MSG_ERR_NONE == ret)
    {
        *out = parser->encode_buffer;
        *out_len = parser->encode_offset;
    }

    return ret;
}