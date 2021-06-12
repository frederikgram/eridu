CC = gcc
DEPS = eridu.h
OBJS = parser.o
parser: $(OBJS) 
	$(CC) -o $@ $^
.PHONY: clean
clean:
	rm -f *.o parser
