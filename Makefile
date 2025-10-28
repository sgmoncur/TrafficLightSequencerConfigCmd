CC=gcc
CFLAGS=


#%.o: %.c
#	$(CC) -c -o $@

main.o: main.c
	$(CC) -c main.c

crc16.o: crc16.c
	$(CC) -c crc16.c

TrafficLightSequencerConfig: main.o crc16.o
	$(CC) -o TrafficLightSequencerConfig main.o crc16.o $(CFLAGS) 

clean:
	rm -f *.o TrafficLightSequencerConfig
