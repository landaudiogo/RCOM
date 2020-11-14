#!/usr/bin/env bash

cd ../../projeto_1
make
cd -
cp ../../projeto_1/*.o ./protocol
cp -r ../../projeto_1/headers/ ./
cp ./headers/linklayer.h ./

gcc main.c ./protocol/*.o -o main
