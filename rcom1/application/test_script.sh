#!/bin/bash

GREEN='\033[32m'
RED='\033[31m'
RESET='\033[0m'


# create result directories
mkdir -p tests/images

image=tests/images

# using a fifo as input for cable
(../cable/cable < /tmp/cable_input 1> tests/cable.log) &
cat <<< "on" > /tmp/cable_input

# start the cable program
printf "${GREEN}STARTING DEFAULT TEST${RESET}\n"
read 

# DEFAULT test
(( test = 0 ))
mkdir -p tests/${test}
./build.sh --default || 
    { echo "Failed Build"; exit 1; }
./main /dev/ttyS11 rx ${image}/received_penguin_${test}.gif 1> tests/${test}/receiver.log &
    ./main /dev/ttyS10 tx penguin.gif 1> tests/${test}/sender.log
    wait $!


# user defined parameters
printf "${GREEN}STARTING USER DEFINED PARAMETERS TEST${RESET}\n"
(( test++ ))
mkdir -p tests/${test}
./build.sh -payload500 -timeout1 -p_error0.1 -retries4 -baud57600 ||
    { echo "Failed Build"; exit 1; }
./main /dev/ttyS11 rx ${image}/received_penguin_${test}.gif 1> tests/${test}/receiver.log &
    ./main /dev/ttyS10 tx penguin.gif 1> tests/${test}/sender.log
    wait $!
#
#
### high ERROR transmission with big PAYLOAD
#printf "${GREEN}STARTING INCREASED ERROR WITH SAME PAYLOAD TEST${RESET}\n"
#(( test++ ))
#mkdir -p tests/${test}
#./build.sh -timeout4 -p_error0.3 -retries3 || exit 1
#./main /dev/ttyS10 tx penguin.gif 2>tests/${test}/sender_error.log | tee tests/${test}/sender.log
#./main /dev/ttyS11 rx ${image}/received_penguin_${test}.gif 1>tests/${test}/receiver.log 2>tests/${test}/receiver_error.log 
#wait
#
#
## high ERROR transmission with small payload
#printf "${GREEN}STARTING INCREASED ERROR WITH SAME PAYLOAD TEST${RESET}\n"
#(( test++ ))
#mkdir -p tests/${test}
#./build.sh -payload50  || exit 1
#./main /dev/ttyS10 tx penguin.gif 2>tests/${test}/sender_error.log | tee tests/${test}/sender.log
#./main /dev/ttyS11 rx ${image}/received_penguin_${test}.gif 1>tests/${test}/receiver.log 2>tests/${test}/receiver_error.log 
##wait
