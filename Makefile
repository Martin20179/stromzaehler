VERSION = 0.01
CC      = g++
#CFLAGS  = -Wall -g -D_REENTRANT -DVERSION=\"$(VERSION)\"
#LDFLAGS = -lm -lpthread `gtk-config --cflags` `gtk-config --libs` -lgthread

OBJ = stromzaehler/main.o stromzaehler/serial.o stromzaehler/http.o

strom: $(OBJ)
	$(CC) $(CFLAGS) -o strom $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm $(OBJ)
	rm strom
