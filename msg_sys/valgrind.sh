#!/bin/bash
cd "$(dirname "$0")"
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -v ./harbol_msg_sys_test
