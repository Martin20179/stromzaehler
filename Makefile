# Makefile fuer den Stromzaehler
# -------------------------------
# Baut aus den Quelldateien das ausfuehrbare Programm "strom".
#   make        -> kompiliert und linkt das Programm
#   make clean  -> entfernt Objektdateien und das Programm

VERSION = 0.01
CC      = g++
# Optionale Compiler-/Linker-Flags (bei Bedarf einkommentieren):
#CFLAGS  = -Wall -g -D_REENTRANT -DVERSION=\"$(VERSION)\"
#LDFLAGS = -lm -lpthread `gtk-config --cflags` `gtk-config --libs` -lgthread

# Objektdateien, aus denen das Programm zusammengesetzt wird
OBJ = stromzaehler/main.o stromzaehler/serial.o stromzaehler/http.o

# Hauptziel: das ausfuehrbare Programm "strom"
strom: $(OBJ)
	$(CC) $(CFLAGS) -o strom $(OBJ) $(LDFLAGS)

# Regel zum Erzeugen der Objektdateien aus den Quelldateien
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Aufraeumen: Objektdateien und Programm loeschen
clean:
	rm $(OBJ)
	rm strom
