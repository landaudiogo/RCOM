#!/bin/bash

################################
########## OVERLOAD ############
################################
# This is for the internal network
enable
conf t
interface gigabit ethernet 0/0                          # interface gigabit slot/port
ip address 172.16.41.254 255.255.255.0                  # this is setting the router's ip address in the aformentioned interface
no shutdown
ip nat inside 
exit

# Defining the external interface
interface gigabit ethernet 0/1                          # interface gigabit slot/port
ip address 172.16.1.49 255.255.255.0                    # this is setting the router's ip address in this interface
no shutdown
ip nat outside
exit

ip nat pool ovrld 172.16.1.49 172.16.1.49 prefix 24     # creating a pool of outward adresses for the router (172.16.1.{19..19})
ip nat inside source list 4 pool ovrld overload         # packets coming from IP's in the access-list 1, their IP's are changed to one of the IP's in the "ovrld" pool
                                                        #   overload just means that a single outward IP address can be used by one or more source IP adresses 

access-list 4 permit 172.16.41.0 0.0.0.7                # the first 7 addresses from 172.16.41.{1..7}
access-list 4 permit 172.16.40.0 0.0.0.7                # the first 7 addresses from 172.16.40.{1..7}

                                                        # Defines the static routes to send the packets for given destination network
ip route 0.0.0.0     0.0.0.0        172.16.1.254        # IP route [prefix] [mask] [ip-address]      (in this case, we are just setting the default hop)
ip route 172.16.40.0 255.255.255.0  172.16.41.253       # prefix     - ip route prefix for destination
                                                        # mask       - prefix mask for destination
                                                        # ip-address - ip address of the next hop that can be used to reach that network
                                                        # 
                                                        # There is no need to set the route for the network the router belongs to (172.16.41.0/24)
