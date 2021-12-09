CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -s -O2
TFLAGS = -Wall -Wextra -pedantic -std=c99 -g -O2

SRCS = deque.c
OBJS = $(SRCS:.c=.o)

harbol_deque:
	$(CC) $(CFLAGS) -c $(SRCS)

debug:
	$(CC) $(TFLAGS) -c $(SRCS)

test:
	$(CC) $(TFLAGS) $(SRCS) test_deque.c -o harbol_deque_test

clean:
	$(RM) *.o
	$(RM) harbol_deque_test
	$(RM) harbol_deque_output.txt

run_test:
	./harbol_deque_test
