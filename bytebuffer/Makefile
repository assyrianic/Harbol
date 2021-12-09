CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -s -O2
TFLAGS = -Wall -Wextra -pedantic -std=c99 -g -O2

SRCS = bytebuffer.c
SRCS += ../array/array.c
OBJS = $(SRCS:.c=.o)

harbol_bytebuffer:
	$(CC) $(CFLAGS) -c $(SRCS)

debug:
	$(CC) $(TFLAGS) -c $(SRCS)

test:
	$(CC) $(TFLAGS) $(SRCS) test_bytebuffer.c -o harbol_bytebuffer_test

clean:
	$(RM) *.o
	$(RM) harbol_bytebuffer_test
	$(RM) harbol_bytebuffer_output.txt

run_test:
	./harbol_bytebuffer_test
