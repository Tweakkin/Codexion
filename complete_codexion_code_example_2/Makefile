NAME = codexion
CC = cc
CFLAGS = -Wall -Werror -Wextra -pthread

SRCS = src/main.c src/parser.c src/pqueue.c src/coder_actions.c \
	src/coder_routine.c src/take_dongles.c src/drop_dongles.c \
	src/init.c src/monitor.c src/simulation.c src/time_utils.c \
	src/take_dongles_utils.c
OBJS = $(SRCS:.c=.o)

HEADER = src/codexion.h

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re