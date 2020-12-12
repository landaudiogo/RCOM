#!/bin/bash

ifconfig eth0 172.16.40.1/24
route add default gw 172.16.40.254              # setting tux4 as default gw

echo "finished configuration"
