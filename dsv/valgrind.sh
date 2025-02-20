#!/bin/bash
cd "$(dirname "$0")"
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -v ./harbol_dsv_test |& tee dsv_valgrind_output.txt
