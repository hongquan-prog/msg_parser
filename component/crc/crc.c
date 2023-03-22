#include "crc.h"

uint8_t crc8(uint8_t init, const uint8_t *data, int len)
{
    uint8_t crc = init;

    for (int i = 0; i < len; i++)
    {
        crc ^= data[i];

        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 0x80)
            {
                crc <<= 1;
                crc ^= 0x07;
            }
            else
            {
                crc <<= 1;
            }
        }
    }

    return crc;
}