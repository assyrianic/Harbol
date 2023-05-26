#CC = clang-11.0
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -Warray-parameter=0 -s -O2
TFLAGS = -Wall -Wextra -pedantic -std=c99 -Warray-parameter=0 -g -O2
# -fsanitize=undefined 

LIB_NAME = harbol

SRCS  = str/str.c
SRCS += array/array.c
SRCS += tuple/tuple.c
SRCS += bytebuffer/bytebuffer.c
SRCS += map/map.c
SRCS += allocators/mempool/mempool.c
SRCS += allocators/objpool/objpool.c
SRCS += allocators/region/region.c
SRCS += allocators/bistack/bistack.c
SRCS += tree/tree.c
SRCS += variant/variant.c
SRCS += cfg/cfg.c
SRCS += plugins/plugins.c
SRCS += deque/deque.c
SRCS += lex/lex.c
SRCS += msg_sys/msg_sys.c

OBJS = $(SRCS:.c=.o)

harbol_static:
	+$(MAKE) -C str
	+$(MAKE) -C array
	+$(MAKE) -C tuple
	+$(MAKE) -C bytebuffer
	+$(MAKE) -C map
	+$(MAKE) -C allocators/mempool
	+$(MAKE) -C allocators/objpool
	+$(MAKE) -C allocators/region
	+$(MAKE) -C allocators/bistack
	+$(MAKE) -C tree
	+$(MAKE) -C variant
	+$(MAKE) -C cfg
	+$(MAKE) -C plugins
	+$(MAKE) -C deque
	+$(MAKE) -C lex
	+$(MAKE) -C msg_sys
	ar cr lib$(LIB_NAME).a $(OBJS)

harbol_shared:
	+$(MAKE) -C str
	+$(MAKE) -C array
	+$(MAKE) -C tuple
	+$(MAKE) -C bytebuffer
	+$(MAKE) -C map
	+$(MAKE) -C allocators/mempool
	+$(MAKE) -C allocators/objpool
	+$(MAKE) -C allocators/region
	+$(MAKE) -C allocators/bistack
	+$(MAKE) -C tree
	+$(MAKE) -C variant
	+$(MAKE) -C cfg
	+$(MAKE) -C plugins
	+$(MAKE) -C deque
	+$(MAKE) -C lex
	+$(MAKE) -C msg_sys
	$(CC) -shared -o lib$(LIB_NAME).so $(OBJS)

test:
	+$(MAKE) -C str test
	+$(MAKE) -C array test
	+$(MAKE) -C tuple test
	+$(MAKE) -C bytebuffer test
	+$(MAKE) -C map test
	+$(MAKE) -C allocators/mempool test
	+$(MAKE) -C allocators/objpool test
	+$(MAKE) -C allocators/region test
	+$(MAKE) -C allocators/bistack test
	+$(MAKE) -C tree test
	+$(MAKE) -C variant test
	+$(MAKE) -C cfg test
	+$(MAKE) -C plugins test
	+$(MAKE) -C deque test
	+$(MAKE) -C lex test
	+$(MAKE) -C msg_sys test

debug:
	+$(MAKE) -C str debug
	+$(MAKE) -C array debug
	+$(MAKE) -C tuple debug
	+$(MAKE) -C bytebuffer debug
	+$(MAKE) -C map debug
	+$(MAKE) -C allocators/mempool debug
	+$(MAKE) -C allocators/objpool debug
	+$(MAKE) -C allocators/region debug
	+$(MAKE) -C allocators/bistack debug
	+$(MAKE) -C tree debug
	+$(MAKE) -C variant debug
	+$(MAKE) -C cfg debug
	+$(MAKE) -C plugins debug
	+$(MAKE) -C deque debug
	+$(MAKE) -C lex debug
	+$(MAKE) -C msg_sys debug
	ar cr lib$(LIB_NAME).a $(OBJS)

debug_shared:
	+$(MAKE) -C str debug
	+$(MAKE) -C array debug
	+$(MAKE) -C tuple debug
	+$(MAKE) -C bytebuffer debug
	+$(MAKE) -C map debug
	+$(MAKE) -C allocators/mempool debug
	+$(MAKE) -C allocators/objpool debug
	+$(MAKE) -C allocators/region debug
	+$(MAKE) -C allocators/bistack debug
	+$(MAKE) -C tree debug
	+$(MAKE) -C variant debug
	+$(MAKE) -C cfg debug
	+$(MAKE) -C plugins debug
	+$(MAKE) -C deque debug
	+$(MAKE) -C lex debug
	+$(MAKE) -C msg_sys debug
	$(CC) -shared -o lib$(LIB_NAME).so $(OBJS)

clean:
	+$(MAKE) -C str clean
	+$(MAKE) -C array clean
	+$(MAKE) -C tuple clean
	+$(MAKE) -C bytebuffer clean
	+$(MAKE) -C map clean
	+$(MAKE) -C allocators/mempool clean
	+$(MAKE) -C allocators/objpool clean
	+$(MAKE) -C allocators/region clean
	+$(MAKE) -C allocators/bistack clean
	+$(MAKE) -C tree clean
	+$(MAKE) -C variant clean
	+$(MAKE) -C cfg clean
	+$(MAKE) -C plugins clean
	+$(MAKE) -C deque clean
	+$(MAKE) -C lex clean
	+$(MAKE) -C msg_sys clean
	$(RM) *.o

run_test:
	+$(MAKE) -C str run_test
	+$(MAKE) -C array run_test
	+$(MAKE) -C tuple run_test
	+$(MAKE) -C bytebuffer run_test
	+$(MAKE) -C map run_test
	+$(MAKE) -C allocators/mempool run_test
	+$(MAKE) -C allocators/objpool run_test
	+$(MAKE) -C allocators/region run_test
	+$(MAKE) -C allocators/bistack run_test
	+$(MAKE) -C tree run_test
	+$(MAKE) -C variant run_test
	+$(MAKE) -C cfg run_test
	+$(MAKE) -C plugins run_test
	+$(MAKE) -C deque run_test
	+$(MAKE) -C lex run_test
	+$(MAKE) -C msg_sys run_test
	$(RM) *.o
