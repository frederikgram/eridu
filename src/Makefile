CC = gcc
DEPS = lexer.h enums.h tokens.h
OBJS = lexer.o 
lexer: $(OBJS)
	$(CC) -o $@ $^

.PHONY: clean
clean:
	rm -f *.o lexer
