TARGET	= gboy
CC		= clang
CFLAGS	= -g -DDEBUG -std=gnu11 -Wall -Wextra -Wpedantic -Werror
LD		= clang
LFLAGS	= -Wall -Wextra -Werror
LIBS	= -lSDL2

SOURCES	:= $(wildcard src/*.c)
OBJECTS	:= $(patsubst %.c,%.o,$(SOURCES))

ROM_TETRIS 	= ./roms/tetris_rev_a.gb
ROM_DRMARIO = ./roms/dr_mario.gb
ROM_INSTRS	= ./roms/cpu_instrs/cpu_instrs.gb

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) $(LFLAGS) -o $@ $^ $(LIBS)

src/%.o : src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(TARGET) $(OBJECTS)

test: test-tetris

test-tetris: $(TARGET)
	./$(TARGET) $(ROM_TETRIS)

test-drmario: $(TARGET)
	./$(TARGET) $(ROM_DRMARIO)

test-instrs: $(TARGET)
	./$(TARGET) $(ROM_INSTRS)

valgrind: $(TARGET)
	valgrind --leak-check=full ./$(TARGET) $(TESTROM)

static: clean
	scan-build make

.PHONY: all clean test test-tetris test-drmario test-instrs valgrind static
