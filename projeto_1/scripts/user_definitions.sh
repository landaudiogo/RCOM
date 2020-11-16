#!/bin/bash
((i = 0))
for arg_content in "$@"; do
    p_error='^(-p_error)(.*)$'
    payload='^(-payload)(.*)$'
    baud='^(-baud)(.*)$'
    timeout='^(-timeout)(.*)$'
    retries='^(-retries)(.*)$'
    help='^(--help)$'
    default='^--default$'
    if [[ "$arg_content" =~ $p_error ]]; then
        p_error_argument=${BASH_REMATCH[2]}
        ((i++))
    elif [[ "$arg_content" =~ $payload ]]; then
        payload_argument=${BASH_REMATCH[2]}
        ((i++))
    elif [[ "$arg_content" =~ $baud ]]; then
        baud_argument=${BASH_REMATCH[2]}
        ((i++))
    elif [[ "$arg_content" =~ $timeout ]]; then
        timeout_argument=${BASH_REMATCH[2]}
        ((i++))
    elif [[ "$arg_content" =~ $retries ]]; then
        retries_argument=${BASH_REMATCH[2]}
        ((i++))
    elif [[ "$arg_content" =~ $help ]]; then
        display_help=1
        ((i++))
    elif [[ "$arg_content" =~ $default ]]; then 
        p_error_argument=0
        payload_argument=1000
        baud_argument=38400
        timeout_argument=4
        retries_argument=3
        ((i++))
        break;
    fi

done

if [[ $display_help == 1 ]] || [[ $i == 0 ]]; then
    cat << EOF

the appropriate way to run this script is:
./user_definition.sh [ OPTIONS ]

OPTIONS:
    
    -p_error(value) : Inserts value in P_ERROR definition in linklayer.h

    -payload(value) : Inserts value in MAX_PAYLOAD_SIZE definition in linklayer.h

    -baud(value) : Inserts value in BAUDRATE_DEFAULT definition in linklayer.h

    -timeout(value) : Inserts value in TIMEOUT_DEFAULT definition in linklayer.h

    -retries(value) : Inserts value in MAX_RETRANSMISSIONS_DEFAULT definition in linklayer.h

    --default : use the default values in linklayer.h. 
                In case this option is chosen, any other options are ignored

    --help : displays this same page

EOF
    exit 1;
fi


# defining P_ERROR
if [[ ! -z $p_error_argument ]]; then 
    ./define_P_ERROR.sh $p_error_argument
    printf "\n"
fi
if [[ ! -z $payload_argument ]]; then 
    ./define_PAYLOAD.sh $payload_argument
    printf "\n"
fi
if [[ ! -z $baud_argument ]]; then 
    ./define_BAUDRATE.sh $baud_argument
    printf "\n"
fi
if [[ ! -z $timeout_argument ]]; then 
    ./define_TIMEOUT.sh $timeout_argument
    printf "\n"
fi
if [[ ! -z $retries_argument ]]; then 
    ./define_RETRIES.sh $retries_argument
    printf "\n"
fi


YELLOW='\033[1;33m'
RESET='\033[0m'
printf "\n${YELLOW}DEFINED VARIABLES THE SCRIPT WILL RUN WITH\n${RESET}"
grep TIMEOUT_DEFAULT ../headers/linklayer.h | cat
grep MAX_RETRANSMISSIONS_DEFAULT ../headers/linklayer.h | cat
grep P_ERROR ../headers/linklayer.h | cat
grep MAX_PAYLOAD_SIZE ../headers/linklayer.h | cat
grep BAUDRATE_DEFAULT ../headers/linklayer.h | cat
printf "\n"
