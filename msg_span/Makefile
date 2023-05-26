CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -Warray-parameter=0 -s -O2
TFLAGS = -Wall -Wextra -pedantic -std=c99 -Warray-parameter=0 -g -O2

SRCS = msg_span.c
SRCS += ../array/array.c
SRCS += ../str/str.c
SRCS += ../lex/lex.c
OBJS = $(SRCS:.c=.o)

harbol_msg_span:
	$(CC) $(CFLAGS) -c $(SRCS)

debug:
	$(CC) $(TFLAGS) -c $(SRCS)

test:
	$(CC) $(TFLAGS) $(SRCS) test_msg_span.c -o harbol_msg_span_test

clean:
	$(RM) *.o
	$(RM) harbol_msg_span_test
	$(RM) harbol_msg_span_output.txt

run_test:
	./harbol_msg_span_test
