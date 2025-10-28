#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include "crc16.h"

#define BAUDRATE B115200
#define DEVICE "/dev/ttyACM0"

#define FAIL_OPEN -1

struct termios oldtio, currenttio;

static void signalHandler(int signo)
{
	switch(signo)
	{
	case SIGHUP:
		exit(1);
		break;
	default:
		break;
	}
}

int connectToDevice(void)
{
	int fd;

	fd = open(DEVICE, O_RDWR | O_NOCTTY);
	if (fd < 0)
	{
		return FAIL_OPEN;
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

size_t getPacket(char *command, char *packetbuf)
{
	unsigned short crc;

	crc = crc16(command, strlen(command));
	return sprintf(packetbuf, "@%s:%04X\r\n", command, crc);
}

size_t writeToDevice(int fd, const char *command)
{
	return write(fd, command, strlen(command));
}

int readFromDevice(int fd, char *buffer, size_t bufferSize)
{
	return read(fd, buffer, bufferSize);
}

int disconnectFromDevice(int fd)
{
	tcsetattr(fd, TCSANOW, &oldtio);
	close(fd);
}

int devicefd;
char readBuffer[255];

int main(void)
{
	struct sigaction sa;
	char buf[256];
	size_t nread, npacket;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = signalHandler;
	sigaction(SIGHUP, &sa, NULL);


	printf("Connecting... ");
	devicefd = connectToDevice();
	if (devicefd < 0)
	{
		printf("Failed to open device\r\n");
		exit(-1);
	}
	printf("Connected\r\n");

	npacket = getPacket("STATUS", buf);
	if (npacket > 0) {
		if (writeToDevice(devicefd, buf) < 0)
		{
			printf("Write to device failed\r\n");
		}
	}
	else
	{
		printf("Failed to get packet");
	}

	nread = readFromDevice(devicefd, buf, sizeof(buf));
	if (nread < 0)
	{
		printf("Read from device failed\r\n");
	}
	else
	{
		buf[nread] = 0;
		printf(buf);
		printf("\r\n");
	}


	//while(1) {
	//	sleep(1);
	//}

	
	disconnectFromDevice(devicefd);
	return 0;
}
