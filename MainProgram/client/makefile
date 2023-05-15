CC = gcc
CFLAGS = -Wall -Wextra 
LDFLAGS = -lwiringPi -lbcm2835 -lwiringPiDev

SRC = ecoflow.c RFID/value.c RFID/config.c RFID/rfid.c RFID/rc522.c RFID/rfid_sensor.c
OBJ = $(SRC:.c=.o)
EXEC = ecoflow

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(EXEC)
