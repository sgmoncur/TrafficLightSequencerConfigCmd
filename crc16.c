#include "crc16.h"

static char const hex_lookup[17] = "0123456789ABCDEF";

/**
 * Calculate a 16-bit CRC over binary data.
 * @param data the data to be summed
 * @param size the length of data
 * @return a CRC16 checksum
 */
uint16_t crc16(const uint8_t *data, size_t size)
{
    uint16_t crc = 0;
    while (size--)
    {
        crc ^= *data++;
        for (unsigned k = 0; k < 8; k++)
            crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
    }
    return crc;
}

/**
 * 
 * @param crc
 * @param crcString
 * @return 
 */
char *crc16_string(const uint16_t crc, char *crc_string)
{
    crc_string[0] = hex_lookup[(crc & 0xF000) >> 12];
    crc_string[1] = hex_lookup[(crc & 0x0F00) >> 8];
    crc_string[2] = hex_lookup[(crc & 0x00F0) >> 4];
    crc_string[3] = hex_lookup[(crc & 0x000F)];
    crc_string[4] = '\0';
    return crc_string;
}