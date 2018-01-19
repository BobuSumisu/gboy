TARGET	= gboy
CC		= clang
CFLAGS	= -std=c11 -Wall -Wextra -Wpedantic -Werror
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

test: CFLAGS += -g -DDEBUG -DTEST
test: $(TARGET)
	./$(TARGET)

.PHONY: all clean test
