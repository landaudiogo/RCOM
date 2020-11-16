#!/bin/bash


# create result directories
mkdir -p tests/images

image=tests/images

(( test = 0 ))
mkdir -p tests/${test}
./build.sh --default || exit 1
./main /dev/ttyS10 tx penguin.gif > tests/${test}/sender.log &
./main /dev/ttyS11 rx ${image}/received_penguin_${test}.gif > tests/${test}/receiver.log
wait

(( test++ ))
mkdir -p tests/${test}
./build.sh -payload10 || exit 1
./main /dev/ttyS10 tx penguin.gif > tests/${test}/sender.log &
./main /dev/ttyS11 rx ${image}/received_penguin_${test}.gif > tests/${test}/receiver.log
wait

