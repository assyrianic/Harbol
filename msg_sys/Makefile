CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -s -Warray-parameter=0 -O2
TFLAGS = -Wall -Wextra -pedantic -std=c99 -Warray-parameter=0 -g -O2

SRCS = msg_sys.c
OBJS = $(SRCS:.c=.o)

harbol_msg_sys:
	$(CC) $(CFLAGS) -c $(SRCS)

debug:
	$(CC) $(TFLAGS) -c $(SRCS)

test:
	$(CC) $(TFLAGS) $(SRCS) test_msg_sys.c -o harbol_msg_sys_test

clean:
	$(RM) *.o
	$(RM) harbol_msg_sys_test
	$(RM) harbol_msg_sys_output.txt

run_test:
	./harbol_msg_sys_test
