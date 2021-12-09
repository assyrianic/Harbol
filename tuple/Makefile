CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -s -O2
TFLAGS = -Wall -Wextra -pedantic -std=c99 -g -O2

SRCS = tuple.c
SRCS += ../array/array.c
OBJS = $(SRCS:.c=.o)

harbol_tuple:
	$(CC) $(CFLAGS) -c $(SRCS)

debug:
	$(CC) $(TFLAGS) -c $(SRCS)

test:
	$(CC) $(TFLAGS) $(SRCS) test_tuple.c -o harbol_tuple_test

clean:
	$(RM) *.o
	$(RM) harbol_tuple_test
	$(RM) harbol_tuple_output.txt

run_test:
	./harbol_tuple_test
