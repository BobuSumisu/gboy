TARGET	= gboy
CC		= clang
CFLAGS	= -g -DDEBUG -std=gnu11 -Wall -Wextra -Wpedantic -Werror
LD		= clang
LFLAGS	= -Wall -Wextra -Werror
LIBS	= -lSDL2

SOURCES	:= $(wildcard src/*.c)
OBJECTS	:= $(patsubst %.c,%.o,$(SOURCES))

TESTROM0 = ./roms/tetris_rev_a.gb
TESTROM1 = ./roms/dr_mario.gb
TESTROM2 = ./roms/cpu_instrs/cpu_instrs.gb

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) $(LFLAGS) -o $@ $^ $(LIBS)

src/%.o : src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(TARGET) $(OBJECTS)

test: $(TARGET)
	./$(TARGET) $(TESTROM0)

test-1: $(TARGET)
	./$(TARGET) $(TESTROM1)

test-2: $(TARGET)
	./$(TARGET) $(TESTROM2)

valgrind: $(TARGET)
	valgrind --leak-check=full ./$(TARGET) $(TESTROM)

static: clean
	scan-build make

.PHONY: all clean test test-1 test-2 valgrind static
