#!/bin/bash

./compile_run.sh
valgrind --tool=memcheck ./main
