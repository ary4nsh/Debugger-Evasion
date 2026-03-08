#!/bin/sh
cd "$(dirname "$0")"
g++ -Wall -Wextra -o ptrace_traceme ptrace_traceme.cpp
