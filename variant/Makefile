CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -s -O2
TFLAGS = -Wall -Wextra -pedantic -std=c99 -g -O2

SRCS = variant.c
OBJS = $(SRCS:.c=.o)

harbol_variant:
	$(CC) $(CFLAGS) -c $(SRCS)

debug:
	$(CC) $(TFLAGS) -c $(SRCS)

test:
	$(CC) $(TFLAGS) $(SRCS) test_variant.c -o harbol_variant_test

clean:
	$(RM) *.o
	$(RM) harbol_variant_test
	$(RM) harbol_variant_output.txt

run_test:
	./harbol_variant_test
