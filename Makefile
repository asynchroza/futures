CC = gcc
CFLAGS = -Wall -g   # enable all warnings and debugging info
OBJFILES = main.o 
EXEC = futures

all: $(EXEC)

$(EXEC): $(OBJFILES)
	$(CC) $(OBJFILES) -o $(EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJFILES) $(EXEC)

.PHONY: all clean

