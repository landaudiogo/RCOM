#!/bin/bash

############################################
########       RUNNING IN TUX      #########
############################################
route del default gw 172.16.4y.254                          # remove a defined default gateway
route add -net 172.16.4y.0/24 gw 172.16.4y.253              # add a route to a network
route del -net 172.16.4y.0/24 gw 172.16.4y.253              # remove a route for a network
route -n                                                    # show the routes

arp -a                                                      # show an arp table 

echo 0 > /proc/sys/net/ipv4/conf/eth0/accept_redirects      # disable ICMP redirects 
echo 0 > /proc/sys/net/ipv4/conf/all/accept_redirects       # disable ICMP redirects 

echo 1 > /proc/sys/net/ipv4/ip_forward                      # allows routing
echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts     # does not respond to a broadcast ping request in our case 172.16.40.255


############################################
#########         SWTICH          ##########
############################################
show vlan                                                   # show the vlans
no vlan <number>                                            # remove a vlan <number>




############################################
#########         ROUTER          ##########
############################################
no ip route 172.16.40.0 255.255.255.0                       # remove a route to a network
show ip route                                               # show the routing table of the router
