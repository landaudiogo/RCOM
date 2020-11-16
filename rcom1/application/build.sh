#!/usr/bin/env bash
RED='\033[0;31m'
RESET='\033[0m'

#define starting directory absolute
cd "$RCOM_professor"

# Compiling object files
cd ../../projeto_1/scripts
./user_definitions.sh $@ || exit 1
cd ../
make

# Compiling application
cd ../rcom1/application
cp ../../projeto_1/*.o ./protocol
cp -r ../../projeto_1/headers/ ./
gcc main.c ./protocol/*.o -o main
