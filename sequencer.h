/* 
 * File:   sequencer.h
 * Author: smoncur
 *
 * Created on October 30, 2025
 */

#ifndef SEQUENCER_H
#define	SEQUENCER_H

#include <stddef.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define MSG_SEP_START '@'
#define MSG_SEP_RESPONSE '.'
#define MSG_SEP_CRC ':'

#define SEQ_OK 0
#define SEQ_FAIL_OPEN -1
#define SEQ_INVALID_FORMAT -2
#define SEQ_INVALID_CRC -3

int connectToDevice(void);
int disconnectFromDevice(int fd);

int sendMessage(int devicefd, char *command);
int receiveMessage(int devicefd, char *response, size_t responsesize);

#ifdef	__cplusplus
}
#endif

#endif	/* SEQUENCER_H */