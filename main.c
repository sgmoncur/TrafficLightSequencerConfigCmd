#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "sequencer.h"


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

	sendMessage(devicefd, "STATUS");

	nread = receiveMessage(devicefd, buf, sizeof(buf));
	if (nread < 0)
	{
		printf("Read from device failed\r\n");
	}
	else
	{
		buf[nread] = 0;
	}

	//while(1) {
	//	sleep(1);
	//}

	
	disconnectFromDevice(devicefd);
	return 0;
}
