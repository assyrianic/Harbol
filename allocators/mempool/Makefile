CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -s -O2
TFLAGS = -Wall -Wextra -pedantic -std=c99 -g -O2

SRCS = mempool.c
SRCS += ../region/region.c
OBJS = $(SRCS:.c=.o)

harbol_mempool:
	$(CC) $(CFLAGS) -c $(SRCS)

debug:
	$(CC) $(TFLAGS) -c $(SRCS)

test:
	$(CC) $(TFLAGS) $(SRCS) test_mempool.c -o harbol_mempool_test

clean:
	$(RM) *.o
	$(RM) harbol_mempool_test
	$(RM) harbol_mempool_output.txt

run_test:
	./harbol_mempool_test
