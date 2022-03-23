CC = gcc
CFLAGS  = -g -Wall -Werror -fsanitize=address

WW = ww

all: $(WW)

$(WW): $(WW).c
	$(CC) $(CFLAGS) -o $(WW) $(WW).c

clean:
	$(RM) $(WW)