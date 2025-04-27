CC = gcc
CFLAGS = -Wall -Wextra -std=c11   # -DDEBUG para ativar mensagens de debug
LDFLAGS = -lm
TARGET = prob_sched

SRCS = main.c process.c scheduler.c stats.c random_generator.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)


.PHONY: all clean