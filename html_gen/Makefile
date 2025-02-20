CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -s -Warray-parameter=0 -O2
TFLAGS = -Wall -Wextra -pedantic -std=c99 -Warray-parameter=0 -g -O2

SRCS = html_gen.c
OBJS = $(SRCS:.c=.o)

harbol_html_gen:
	$(CC) $(CFLAGS) -c $(SRCS)

debug:
	$(CC) $(TFLAGS) -c $(SRCS)

test:
	$(CC) $(TFLAGS) $(SRCS) test_$(SRCS) -o harbol_html_gen_test

clean:
	$(RM) *.o
	$(RM) harbol_html_gen_test
	$(RM) *.txt

run_test:
	./harbol_html_gen_test
