CC       ?= cc
CFLAGS   ?= -Wall -Wextra -std=c11
CPPFLAGS ?=
LDFLAGS  ?=

TARGET = http_server
SRCS   = server.c parser.c http.c
OBJS   = $(SRCS:.c=.o)
HDRS   = http.h parser.h

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c $(HDRS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) $(PORT)

clean:
	rm -f $(OBJS) $(TARGET)
