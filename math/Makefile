CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -s -Warray-parameter=0 -O2
TFLAGS = -Wall -Wextra -pedantic -std=c99 -Warray-parameter=0 -g -O2

SRCS = math_parser.c
SRCS += ../str/str.c
SRCS += ../lex/lex.c
OBJS = $(SRCS:.c=.o)

harbol_math:
	$(CC) $(CFLAGS) -c $(SRCS)

debug:
	$(CC) $(TFLAGS) -c $(SRCS)

test:
	$(CC) $(TFLAGS) $(SRCS) test_math.c -o harbol_math_test -lm

clean:
	$(RM) *.o
	$(RM) harbol_math_test
	$(RM) harbol_math_output.txt

run_test:
	./harbol_math_test
