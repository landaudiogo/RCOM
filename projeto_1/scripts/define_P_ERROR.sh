#!/bin/bash
re='^([0-9]+(\.[0-9]{0,3})?)$'
if ! [[ $1 =~ $re ]]; then
    echo "P_ERROR argument is not a number with two decimal points floating precision" >&2; exit 1
else
    p_error=${BASH_REMATCH[1]}
    echo "BYTE ERROR PROB: $p_error"
fi


if [[ $(awk "BEGIN {print ($p_error>=0 && $p_error<50)}") == 0 ]]; then
    echo "the first argument belongs to the interval [0, 50[ with 3 decimal point precision"; exit 1
fi

p_error=$(awk "BEGIN {print ($p_error*1000)}")
sed -Ei "s/(^#define\s+P_ERROR\s+)(\w+)/\1$p_error/1" ../headers/linklayer.h
grep P_ERROR ../headers/linklayer.h | cat
