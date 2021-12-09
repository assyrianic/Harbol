CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -s -O2
TFLAGS = -Wall -Wextra -pedantic -std=c99 -g -O2

SRCS = lex.c
SRCS += ../str/str.c
OBJS = $(SRCS:.c=.o)

harbol_lex:
	$(CC) $(CFLAGS) -c $(SRCS)

debug:
	$(CC) $(TFLAGS) -c $(SRCS)

test:
	$(CC) $(TFLAGS) $(SRCS) test_lex.c -o harbol_lex_test

clean:
	$(RM) *.o
	$(RM) harbol_lex_test
	$(RM) harbol_lex_output.txt

run_test:
	./harbol_lex_test
