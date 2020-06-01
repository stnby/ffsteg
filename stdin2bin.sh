#!/bin/sh
xxd -b -c 1 | awk '{print $2}' | tr -d "\n"
echo
