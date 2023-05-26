CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -s -Warray-parameter=0 -O2
TFLAGS = -Wall -Wextra -pedantic -std=c99 -Warray-parameter=0 -g -O2

SRCS = bistack.c
OBJS = $(SRCS:.c=.o)

harbol_bistack:
	$(CC) $(CFLAGS) -c $(SRCS)

debug:
	$(CC) $(TFLAGS) -c $(SRCS)

test:
	$(CC) $(TFLAGS) $(SRCS) test_bistack.c -o harbol_bistack_test

clean:
	$(RM) *.o
	$(RM) harbol_bistack_test
	$(RM) harbol_bistack_output.txt

run_test:
	./harbol_bistack_test
