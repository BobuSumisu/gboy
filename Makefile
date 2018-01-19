TARGET	= gboy
CC		= clang
CFLAGS	= -g -DDEBUG -std=c11 -Wall -Wextra -Wpedantic -Werror
LD		= clang
LFLAGS	= -Wall -Wextra -Werror
LIBS	=

SOURCES	:= $(wildcard src/*.c)
OBJECTS	:= $(patsubst %.c,%.o,$(SOURCES))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) $(LFLAGS) -o $@ $^ $(LIBS)

src/%.o : src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(TARGET) $(OBJECTS)

test: $(TARGET)
	./$(TARGET)

.PHONY: all clean test
