#!/bin/bash

ifconfig eth0 172.16.41.1/24
route add default gw 172.16.41.254              # setting Rc as default gw

echo "finished configuration"
