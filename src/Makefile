CC = gcc
DEPS = tokens.h ast.h
OBJS = parser.o
parser: $(OBJS) 
	$(CC) -o $@ $^
.PHONY: clean
clean:
	rm -f *.o parser
