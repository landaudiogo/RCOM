#!/bin/bash
re='(^[0-9]+)$'
if ! [[ $1 =~ $re ]]; then
    echo "RETRIES argument has to be an Integer" >&2; exit 1
else
    retries=${BASH_REMATCH[1]}
    echo "RETRIES: $retries"
fi

sed -Ei "s/(^#define\s+MAX_RETRANSMISSIONS_DEFAULT\s+)(\w+)/\1$retries/1" ../headers/linklayer.h
grep MAX_RETRANSMISSIONS_DEFAULT ../headers/linklayer.h | cat
