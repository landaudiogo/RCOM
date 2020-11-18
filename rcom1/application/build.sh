#!/usr/bin/env bash
RED='\033[0;31m'
RESET='\033[0m'

#define starting directory absolute
cd "$RCOM_professor"

# Compiling object files
cd ../../projeto_1/scripts && ./user_definitions.sh $@ || 
    { echo "Failed to run user_definitions"; exit 1; }
cd ../ && make || 
    { echo "Failed to run Makefile"; exit 1; }

# Compiling application
cd ../rcom1/application && 
    cp ../../projeto_1/*.o ./protocol &&
    cp -r ../../projeto_1/headers/ ./ || 
    { echo "Failed to copy files"; exit 1; }
gcc main.c ./protocol/*.o -o main || 
    { echo "Failed to compile"; exit 1; }
