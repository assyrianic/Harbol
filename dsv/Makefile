CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -s -Warray-parameter=0 -O2
TFLAGS = -Wall -Wextra -pedantic -std=c99 -Warray-parameter=0 -g -O2

SRCS = dsv.c
SRCS += ../str/str.c
SRCS += ../array/array.c
SRCS += ../msg_span/msg_span.c
SRCS += ../lex/lex.c
OBJS = $(SRCS:.c=.o)

harbol_dsv:
	$(CC) $(CFLAGS) -c $(SRCS)

debug:
	$(CC) $(TFLAGS) -c $(SRCS)

test:
	$(CC) $(TFLAGS) $(SRCS) test_dsv.c -o harbol_dsv_test

clean:
	$(RM) *.o
	$(RM) harbol_dsv_test
	$(RM) *.txt

run_test:
	./harbol_dsv_test
