#!/bin/bash
re='^([0-9]+)$'
if ! [[ $1 =~ $re ]]; then
    echo "TIMEOUT argument has to be an Integer" >&2; exit 1
else
    timeout=${BASH_REMATCH[1]}
    echo "TIMEOUT: $timeout seconds"
fi

sed -Ei "s/(^#define\s+TIMEOUT_DEFAULT\s+)(\w+)/\1$timeout/1" ../headers/linklayer.h
grep TIMEOUT_DEFAULT ../headers/linklayer.h | cat
