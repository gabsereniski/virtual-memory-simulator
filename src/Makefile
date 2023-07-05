CC = gcc
CFLAGS = -Wall -Werror
EXECUTABLE = sim
INPUT_GENERATOR = input_generator

SRCS = globals.c queue.c mmu.c main.c

all: $(EXECUTABLE) $(INPUT_GENERATOR)

$(EXECUTABLE): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(EXECUTABLE)

$(INPUT_GENERATOR): input_generator.c
	$(CC) $(CFLAGS) input_generator.c -o $(INPUT_GENERATOR)

clean:
	rm -f $(EXECUTABLE)
	rm -f $(INPUT_GENERATOR)
	rm -f trace*

.PHONY: all clean
