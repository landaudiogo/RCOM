
enable                  # password 8nortel
conf t
vlan 40                 # creates specified vlan
vlan 41                 # creates specified vlan
end
show vlan               # confirm the vlans have been created


# VLAN0 172.16.40.0/24
int Fa0/1                       # tux3 eth0 Interface
switchport access vlan 40
int Fa0/5                       # tux4 eht0 Interface
switchport access vlan 40


# VLAN1 172.16.41.0/24
int Fa0/2                       # tux2 eth0 Interface 
switchport access vlan 41
int Fa0/9                       # tux4 eth1 Interface
switchport access vlan 41
int Fa0/13                      # Rc Interface
switchport access vlan 41

