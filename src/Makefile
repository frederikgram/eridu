CC = gcc
DEPS = lexer.h ast.h parser.h
OBJS = lexer.o parser.o
parser: $(OBJS)
	$(CC) -o $@ $^

.PHONY: clean
clean:
	rm -f *.o parser
