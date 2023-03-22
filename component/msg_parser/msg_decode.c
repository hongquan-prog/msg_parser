#include "msg_parser.h"
#include "crc.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>

void msg_decoder_init(msg_parser_t *parser)
{
    parser->remain = sizeof(msg_packet_header_t);
    parser->recved_start = 0;
    parser->recved_header = 0;
    parser->decode_msg = NULL;
    parser->decode_overflow = 0;
}

static msg_packet_t *msg_decode_verify(msg_parser_t *parser)
{
    uint8_t crc = 0;

    if (parser->decode_msg)
    {
        if (!parser->decode_overflow)
        {
            crc = crc8(0, (uint8_t *)&(parser->decode_msg->header), sizeof(msg_packet_header_t));
            crc = crc8(crc, parser->decode_msg->payload, parser->decode_msg->header.data_length);

            if (parser->decode_msg->tail.crc != crc)
            {
                parser->decode_msg = NULL;
            }
        }
        else
        {
            parser->decode_msg = NULL;
        }
    }

    return parser->decode_msg;
}

static msg_packet_t *msg_decode_finish(msg_parser_t *parser)
{
    msg_packet_t *ret = NULL;
    msg_packet_t *temp = parser->decode_msg;

    if (parser->recved_header && !parser->remain)
    {
        temp->payload = parser->decode_buffer;
        temp->tail.crc = parser->decode_buffer[temp->header.data_length];
        ret = msg_decode_verify(parser);
        msg_decoder_init(parser);
    }

    return ret;
}

msg_packet_t *msg_decode_packet(msg_parser_t *parser, uint8_t *data, uint8_t *len)
{
    uint8_t crc = 0;
    msg_packet_t *ret = NULL;

    if ((NULL == data) || (0 == len))
    {
        return ret;
    }

    /* check if the decoder is initialized */
    if (!parser->decoder_init)
    {
        msg_decoder_init(parser);
        parser->decoder_init = 1;
    }

    /* detect start byte */
    if (!parser->recved_start)
    {
        uint8_t *pos = strchr(data, MSG_START);

        /* find the start byte in the data buffer */
        if (pos)
        {
            if (((unsigned long)pos - (unsigned long)data) <= *len)
            {
                /* behind the start byte is header */
                *len = *len - ((unsigned long)pos - (unsigned long)data) - 1;
                /* move the data pointer to the start of the header */
                data = pos + 1;
                parser->recved_start = 1;
            }
        }
        else
        {
            /* discard */
            *len = 0;
        }
    }

    if (parser->recved_start && data && (*len > 0))
    {
        if (parser->recved_header)
        {
            if (parser->decode_msg)
            {
                uint8_t min = (parser->remain < *len) ? (parser->remain) : (*len);
                uint8_t offset = parser->decode_msg->header.data_length + sizeof(msg_packet_tail_t) - parser->remain;

                /* check if there is overflow in the decode buffer */
                if (!parser->decode_overflow)
                {
                    /* copy the received data to the parser's buffer */
                    memcpy(parser->decode_buffer + offset, data, min);
                }

                *len = *len - parser->remain;
                parser->remain -= min;
            }

            ret = msg_decode_finish(parser);
        }
        else
        {
            uint8_t min = (parser->remain < *len) ? (parser->remain) : (*len);
            uint8_t offset = sizeof(msg_packet_header_t) - parser->remain;

            /* copy the received data to the parser's buffer */
            memcpy(parser->decode_buffer + offset, data, min);

            if (min >= parser->remain)
            {
                data += parser->remain;
                *len = *len - parser->remain;
                parser->recved_header = 1;

                 /* store the message header in the parser's cache and set the expected remaining length of the message */
                memcpy(&(parser->decode_cache), parser->decode_buffer, sizeof(msg_packet_header_t));
                parser->decode_msg = &parser->decode_cache;
                parser->remain = parser->decode_msg->header.data_length + sizeof(msg_packet_tail_t);

                if (parser->remain >= MSG_BUFFER_SIZE)
                {
                    // TODO: exception inform
                    parser->decode_overflow = 1;
                }

                /* recursively call the function to receive the message body and tail */
                ret = msg_decode_packet(parser, data, len);
            }
            else
            {
                *len = *len - parser->remain;
                parser->remain -= min;
            }
        }
    }

    return ret;
}

msg_t *msg_decode(msg_parser_t *parser, msg_t *msg, uint8_t *data, uint8_t *len)
{
    msg_packet_t *ret = NULL;

    ret = msg_decode_packet(parser, data, len);

    if (ret)
    {
        msg->data_length = ret->header.data_length;
        msg->payload = ret->payload;
    }

    return (ret) ? (msg) : (NULL);
}

void msg_decode_async(msg_parser_t *parser, uint8_t *data, uint8_t len)
{
    msg_packet_t *ret = NULL;

    while (len)
    {
        ret = msg_decode_packet(parser, data, &len);

        if (ret)
        {
            if (parser->decoder_cb.func)
            {
                parser->decoder_cb.func(parser->decoder_cb.user_data, ret->payload, ret->header.data_length);
            }
        }
    }
}

void msg_register_decode_async_handler(msg_parser_t *parser, msg_callback_t *cb)
{
    if (cb)
    {
        parser->decoder_cb = *cb;
    }
}
