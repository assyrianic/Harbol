#!/bin/bash
cd "$(dirname "$0")"
clear; make clean; make debug; make test; make run_test
