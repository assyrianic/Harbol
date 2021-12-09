CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -s -O2
TFLAGS = -Wall -Wextra -pedantic -std=c99 -g -O2

SRCS = tree.c
SRCS += ../array/array.c
OBJS = $(SRCS:.c=.o)

harbol_tree:
	$(CC) $(CFLAGS) -c $(SRCS)

debug:
	$(CC) $(TFLAGS) -c $(SRCS)

test:
	$(CC) $(TFLAGS) $(SRCS) ../str/str.c test_tree.c -o harbol_tree_test

clean:
	$(RM) *.o
	$(RM) harbol_tree_test
	$(RM) harbol_tree_output.txt

run_test:
	./harbol_tree_test
