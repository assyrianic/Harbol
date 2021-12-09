CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -s -O2
TFLAGS = -Wall -Wextra -pedantic -std=c99 -g -O2

SRCS = str.c
OBJS = $(SRCS:.c=.o)

harbol_string:
	$(CC) $(CFLAGS) -c $(SRCS)

debug:
	$(CC) $(TFLAGS) -c $(SRCS)

test:
	$(CC) $(TFLAGS) $(SRCS) test_$(SRCS) -o harbol_string_test

clean:
	$(RM) *.o
	$(RM) harbol_string_test
	$(RM) harbol_string_output.txt

run_test:
	./harbol_string_test
