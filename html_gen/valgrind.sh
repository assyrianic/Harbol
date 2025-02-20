#!/bin/bash
cd "$(dirname "$0")"
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -v ./harbol_html_gen_test |& tee htmlgen_valgrind_output.txt
