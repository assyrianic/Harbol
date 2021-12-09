CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -s -O2
TFLAGS = -Wall -Wextra -pedantic -std=c99 -g -O2

SRCS = plugins.c
OBJS = $(SRCS:.c=.o)

harbol_plugins:
	$(CC) $(CFLAGS) -c $(SRCS)

debug:
	$(CC) $(TFLAGS) -c $(SRCS)

test:
	$(CC) -Wall -Wextra -std=c99 -g -O2 -shared -c test_harbol_plugins/test_plugin.c
	$(CC) -shared -o test_plugin.so test_plugin.o
	$(CC) -Wall -Wextra -std=c99 -g -O2 -shared -c test_harbol_plugins/plugin_subfolder/plugin.c
	$(CC) -shared -o plugin.so plugin.o
	$(RM) test_plugin.o plugin.o
	mv --force plugin.so test_harbol_plugins/plugin_subfolder
	mv --force test_plugin.so test_harbol_plugins/
	$(CC) $(TFLAGS) $(SRCS) test_plugins.c -o harbol_plugins_test -ldl -Wl,--export-dynamic

clean:
	$(RM) *.o
	$(RM) harbol_plugins_test
	$(RM) harbol_plugins_output.txt

run_test:
	./harbol_plugins_test
