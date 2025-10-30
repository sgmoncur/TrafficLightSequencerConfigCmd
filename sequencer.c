#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "crc16.h"
#include "sequencer.h"
#include <fcntl.h>
#include <termios.h>

#define BAUDRATE B115200
#define DEVICE "/dev/ttyACM0"

static struct termios oldtio, currenttio;

static int getPacket(char *command, char *packetbuf, size_t packetbufsize);
static int writeToDevice(int fd, const char *command);
static int readFromDevice(int fd, char *buffer, size_t bufferSize);
static int validateCrc(char *rxcrc, uint16_t computedCrc);
static int unpackMessage(char *message, char *response, size_t responsesize);

void debugPrintMessageHex(char *msg)
{
    while(*msg)
    {
        printf("%c %02X ", *msg, *msg);
        ++msg;
    }
    printf("\n");
}

static int getPacket(char *command, char *packetbuf, size_t packetbufsize)
{
	return snprintf(packetbuf, packetbufsize, "@%s:%04X\r", command, crc16(command, strlen(command)));
}


static int writeToDevice(int fd, const char *command)
{
	return write(fd, command, strlen(command));
}

static int readFromDevice(int fd, char *buffer, size_t bufferSize)
{
	return read(fd, buffer, bufferSize);
}

static int validateCrc(char *rxcrc, uint16_t computedCrc)
{
    printf("CRC: rx %s cmp %04X\n", rxcrc, computedCrc);
    return 1;
}

static int unpackMessage(char *message, char *response, size_t responsesize)
{
    char *status;
    char *responsePtr;
    char *crcPtr;

    if (message[0] != MSG_SEP_START)
    {
        return SEQ_INVALID_FORMAT;
    }

    if ((crcPtr = strrchr(message, MSG_SEP_CRC)) == NULL)
    {
        printf("No CRC");
        return SEQ_INVALID_FORMAT;
    }
    *crcPtr++ = 0;

    uint16_t crc = crc16(message, strlen(message));
    if (validateCrc(crcPtr, crc) < 0)
    {
        return SEQ_INVALID_CRC;
    }

    status = &message[1];

    if ((responsePtr = strchr(message, MSG_SEP_RESPONSE)) != NULL)
    {
        *responsePtr++ = 0;
        strncpy(response, responsePtr, responsesize);
    }
    printf("Response: status=%s payload=%s\n", status, responsePtr);
    return (strcpy(status, "OK") == 0);
}

/*
    Public API
*/

int connectToDevice(void)
{
	int fd;

	fd = open(DEVICE, O_RDWR | O_NOCTTY);
	if (fd < 0)
	{
		return SEQ_FAIL_OPEN;
	}

	tcgetattr(fd, &oldtio);

	bzero(&currenttio, sizeof(currenttio));
	currenttio.c_cflag = BAUDRATE;
	currenttio.c_iflag = IGNPAR | ICRNL;
	currenttio.c_oflag = 0;
	currenttio.c_lflag = ICANON;
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &currenttio);
	return fd;
}

int disconnectFromDevice(int fd)
{
	tcsetattr(fd, TCSANOW, &oldtio);
	close(fd);
}

int sendMessage(int devicefd, char *command)
{
	char buf[256];

    printf("Command: payload=%s\n", command);

	if (getPacket(command, buf, sizeof buf) < 0)
	{
		return -1;
	}

    printf("TX Packet: %s\n", buf);

	return writeToDevice(devicefd, buf);
}

int receiveMessage(int devicefd, char *response, size_t responsesize)
{
    char buf[256];
    int nread;

	nread = readFromDevice(devicefd, buf, sizeof(buf));
    if (nread < 0)
    {
        return nread;
    }
    buf[nread] = 0;
    buf[strcspn(buf, "\r\n")] = 0;

    printf("RX Packet: %s\n", buf);

    return unpackMessage(buf, response, responsesize);
}