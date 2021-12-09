CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -s -O2
TFLAGS = -Wall -Wextra -pedantic -std=c99 -g -O2

SRCS = array.c
OBJS = $(SRCS:.c=.o)

harbol_array:
	$(CC) $(CFLAGS) -c $(SRCS)

debug:
	$(CC) $(TFLAGS) -c $(SRCS)

test:
	$(CC) $(TFLAGS) $(SRCS) test_$(SRCS) -o harbol_array_test

clean:
	$(RM) *.o
	$(RM) harbol_array_test
	$(RM) harbol_array_output.txt

run_test:
	./harbol_array_test
