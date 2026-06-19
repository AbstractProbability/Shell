# 1. Add Variables
CC = gcc
CFLAGS = -Wall -g -Iinclude -std=c99 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 -Wextra -Werror -Wno-unused-parameter -fno-asm
TARGET = shell.out

# 2. Add .o files
OBJECTS = main.o parser.o cd.o reveal.o jobs.o execute.o

# 3. Add Rules
# Default rule to build the program
all: $(TARGET)

# Rule to link all the object files into the final program
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

# main.o
main.o: src/main.c include/common.h include/execute.h
	$(CC) $(CFLAGS) -c src/main.c

# parser.o
parser.o: src/parser.c include/parser.h include/common.h
	$(CC) $(CFLAGS) -c src/parser.c

# cd.o
cd.o: src/cd.c include/cd.h
	$(CC) $(CFLAGS) -c src/cd.c

# reveal.o
reveal.o: src/reveal.c include/reveal.h
	$(CC) $(CFLAGS) -c src/reveal.c

# jobs.o
jobs.o: src/jobs.c include/jobs.h
	$(CC) $(CFLAGS) -c src/jobs.c

# execute.o
execute.o: src/execute.c include/execute.h
	$(CC) $(CFLAGS) -c src/execute.c


# 3. Cleanup
.PHONY: clean
clean:
	rm -f $(OBJECTS) $(TARGET)

