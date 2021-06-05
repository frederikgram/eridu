CC = gcc
DEPS = tokens.h
OBJS = lexer.o 
lexer: $(OBJS)
	$(CC) -o $@ $^

.PHONY: clean
clean:
	rm -f *.o lexer
