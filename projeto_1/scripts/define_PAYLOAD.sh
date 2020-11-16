#!/bin/bash
re='^([0-9]+)$'
if ! [[ $1 =~ $re ]]; then
    echo "PAYLOAD argument has to be an Integer" >&2; exit 1
else
    payload=${BASH_REMATCH[1]}
    echo "PAYLOAD: $payload"
fi

sed -Ei "s/(^#define\s+MAX_PAYLOAD_SIZE\s+)(\w+)/\1$payload/1" ../headers/linklayer.h
grep MAX_PAYLOAD_SIZE ../headers/linklayer.h | cat
