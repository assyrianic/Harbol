CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -s -O2
TFLAGS = -Wall -Wextra -pedantic -std=c99 -g -O2

SRCS = map.c
SRCS += ../array/array.c
OBJS = $(SRCS:.c=.o)

harbol_map:
	$(CC) $(CFLAGS) -c $(SRCS)

debug:
	$(CC) $(TFLAGS) -c $(SRCS)

test:
	$(CC) $(TFLAGS) $(SRCS) test_map.c -o harbol_map_test

clean:
	$(RM) *.o
	$(RM) harbol_map_test
	$(RM) harbol_map_output.txt

run_test:
	./harbol_map_test
