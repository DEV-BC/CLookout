CC 	= gcc
CFLAGS 	= -std=c11 -Wall -Wextra -Werror -g -fsanitize=undefined -I include
TARGET 	= build/sentinel-c
SRCS 	= src/main.c src/device.c src/incident.c src/todo.c src/chat.c src/colors.c src/layout.c
OBJS 	= $(patsubst src/%.c,build/%.o,$(SRCS))

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p build
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -lncurses -lcjson -lpthread

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f build/*.o $(TARGET)

run: $(TARGET)
		./$(TARGET)



