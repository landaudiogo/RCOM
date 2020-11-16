#!/bin/bash
re='^(0|50|75|110|134|150|200|300|600|1200|1800|2400|4800|9600|19200|38400|57600|115200|230400)$'
if ! [[ $1 =~ $re ]]; then
    echo "BAUDRATE argument has to belong to the following set {0, 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400}" >&2; exit 1
else
    baud=${BASH_REMATCH[1]}
    echo "BAUD RATE: $baud"
fi

sed -Ei "s/(^#define\s+BAUDRATE_DEFAULT\s+)(\w+)/\1B$baud/1" ../headers/linklayer.h
grep BAUDRATE_DEFAULT ../headers/linklayer.h | cat
