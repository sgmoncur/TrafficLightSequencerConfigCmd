CC=gcc
CFLAGS=
APPNAME=tlsc

#%.o: %.c
#	$(CC) -c -o $@

tlsc: main.o crc16.o sequencer.o
	$(CC) -o $(APPNAME) main.o crc16.o sequencer.o $(CFLAGS) 

main.o: main.c
	$(CC) -c main.c

crc16.o: crc16.c
	$(CC) -c crc16.c

sequencer.o: sequencer.c
	$(CC) -c sequencer.c

clean:
	rm -f *.o $(APPNAME)
