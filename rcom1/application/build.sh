#!/usr/bin/env bash

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
