/* 
 * File:   crc16.h
 * Author: smoncur
 *
 * Created on June 30, 2025, 10:55 PM
 */

#ifndef CRC16_H
#define	CRC16_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

uint16_t crc16(const uint8_t *data, size_t size);
char *crc16_string(const uint16_t crc, char *crc_string);


#ifdef	__cplusplus
}
#endif

#endif	/* CRC16_H */