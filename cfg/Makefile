CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -s -O2
TFLAGS = -Wall -Wextra -pedantic -std=c99 -g -O2

SRCS = cfg.c
SRCS += ../msg_sys/msg_sys.c
SRCS += ../lex/lex.c
SRCS += ../variant/variant.c
SRCS += ../map/map.c
SRCS += ../array/array.c
SRCS += ../str/str.c
OBJS = $(SRCS:.c=.o)

harbol_cfg:
	$(CC) $(CFLAGS) -c $(SRCS)

debug:
	$(CC) $(TFLAGS) -c $(SRCS)

test:
	$(CC) $(TFLAGS) $(SRCS) test_cfg.c -o harbol_cfg_test

clean:
	$(RM) *.o
	$(RM) harbol_cfg_test
	$(RM) *.ini
	$(RM) harbol_cfg_output.txt

run_test:
	./harbol_cfg_test
