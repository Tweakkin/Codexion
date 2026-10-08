NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

SRC = src/main.c \
      src/parser/parse.c \
      src/helpers/utils.c \
      src/setup/init.c \
      src/setup/setup.c \
      src/threads/coder_routine.c \
      src/threads/coder_queue.c \
      src/threads/coder_dongles.c \
      src/threads/monitor.c

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c include/codexion.h
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
